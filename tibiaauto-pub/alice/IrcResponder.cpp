#include <stdafx.h>

#include "IrcResponder.h"

#include "StringTokenizer.h"
#include "TokenProxyListener.h"
#include "Kernel.h"
#include "Utils.h"
#include "SocketHandler.h"
#include "Logger.h"
#include "TimeKeeper.h"

#include <strstream>
#include <iostream>


using namespace std;

	class Reconnecter : public Event
	{
	public:
		Reconnecter(Socket *s) {
			sock = s;
		}
		virtual ~Reconnecter() { }
		
		virtual void run() {
			sock->connect();
		}
	private:
		Socket *sock;
	};


class IrcPassThru : public SocketListener, ServerSocketListener
{
public:
	IrcPassThru(IrcResponder *ir) {
		bot = ir;
		
		client = NULL;
		
		server = new ServerSocket(bot->config.ptPort);
		SocketHandler::addSocket(bot->config.description + " passthru", server);
		server->setServerListener(this);
		if (server->init()) {
			cout << "Starting up passthrough server (" << bot->config.ptPort << ")" << endl;
		}
	}
	~IrcPassThru() {
		SocketHandler::removeSocket(bot->config.description + " passthru");
		if (client != NULL) {
			SocketHandler::removeSocket("passthru client");
		}
	}
	
	//	ServerSocketListener
	void awaitingClient(Socket *s) {
		user = nick = false;
		cout << "Pass Through User connected" << endl;
		if (client != NULL) {
			client->disconnect("You have been booted");
			SocketHandler::removeSocket("passthru client");
		}
		client = s;
		client->setListener(new TokenProxyListener(this));
		bot->botControl = false;
		SocketHandler::addSocket("passthru client", client);
	}
	void shutdown(const string &reason) {
		cout << "Pass Through Server shutdown (" << reason << ")" << endl;
	}
	//	SocketListener
	void connected() {
		;
	}
	void disconnected(const string &reason) {
		cout << "Pass Through User disconnected (" << reason << ")" << endl;
		user = nick = false;
		bot->botControl = true;
	}
	void recv(string &in) {
		if (in.find("QUIT :") == 0) {
			//	Don't let bot quit when user quits
			//	This does prevent user forcing bot to quit, however
		}
		else if (!user && in.find("USER") == 0) {
			user = true;
			if (nick) {
				//	Can send the connectCache
				//client->write(bot->connectCache);
				sendWelcome();
			}
		}
		else if (!nick && in.find("NICK") == 0) {
			nick = true;
			if (user) {
				//	Can send the connectCache
				//client->write(bot->connectCache);
				sendWelcome();
			}
		}
		else if(nick && in.find("AITOGGLE") == 0){
			bot->botControl = !bot->botControl ;
			client->write(":GOD 372 "+bot->nick+" :- AI mode is now ");
			if(bot->botControl) {
				client->write("on\r\n");
			} else {
				client->write("off\r\n");
			}
		}
		else if (user && nick) {
			//	FIXME: Track nick changes
			//	Ignore until bot has registered, so
			//	that it doesn't try make bot
			//	reregister and change nicks
			if (in.find("JOIN") == 0) {
				string channel = in.substr(5);
				for (size_t ix = 0; ix < bot->config.channels.size(); ++ix) {
					if (toLower(channel) == toLower(bot->config.channels[ix])) {
						//	Fake the join...hopefully works
						client->write(":" + bot->nick + " JOIN :" + channel + "\r\n");
						//	TOPIC
						bot->send("TOPIC " + channel);
						//	NAMES LIST
						bot->send("NAMES " + channel);
						return;
					}
				}
				bot->config.channels.push_back(channel);
			}
			else if (in.find("PART") == 0) {
				string channel = in.substr(5);
				vector<string>::iterator itr = bot->config.channels.begin();
				for (size_t ix = 0; ix < bot->config.channels.size(); ++ix) {
					if (toLower(channel) == toLower(bot->config.channels[ix])) {
						bot->config.channels.erase(itr);
					}
					++itr;
				}
			}
			bot->send(in, true);
		}
	}
	void send(const string &out) {
		string n = ":" + bot->nick;
		if (out.find(n) == 0) {
			n = " NICK :";
			if (out.find(n) != string::npos) {
				string newnick = trim(out.substr(out.find(n) + n.length()));
				cerr << "Bot's new nick is: '" << newnick << "'" << endl;
				bot->nick = newnick;
			}
		}
		client->write(out + "\r\n");
	}
private:
	Socket *client;
	ServerSocket *server;
	IrcResponder *bot;
	bool user, nick;
	
	//	FIXME: Make this a bit cooler later :)
	void sendWelcome() {
		string welcome = "";
		string head = ":" + bot->serverHost + " ";
		welcome += head + "001 " + bot->nick + " :Welcome to the J-Alice IRC PassThru Server " + bot->nick + "\r\n";
		welcome += head + "375 " + bot->nick + " :- Message of the Day -\r\n";
		welcome += head + "372 " + bot->nick + " :- To view real MOTD, type /motd\r\n";
		welcome += head + "376 " + bot->nick + " :End of /MOTD command.\r\n";
	
		client->write(welcome);
	}
	
	friend class IrcResponder;
};
	

IrcResponder::IrcResponder(IrcConfig &ic) {
	config = ic;
	
	nickIndex = -1;
	changeNick();
	pass = nick + "@j-alice.sourceforge.net";
	retryCount = 0;
	
	ipt = NULL;
	
	start(0);
}

void IrcResponder::connected() {
	retryCount = 0;
	connect();
}

IrcResponder::~IrcResponder() {
	disconnect();
}

string IrcResponder::respond(Match *m, PElement element, const string &id) {
	if (element->getNamespace() != "irc") {
		return "";
	} else {
		string tag = element->getTagname();
		if (tag == "channel") {
			return mChannel;
		} else if (tag == "nick") {
			return mNick;
		} else if (tag == "command") {
			string cmd = Kernel::process(m, element, this, id);
			if (cmd[0] == '/') {
				processAlias(cmd);
			} else {
				q.push(cmd);
			}
		} else if (tag == "exit") {
			disconnect();
			serverRunning = false;	//	Natural quit
		} else {
			cout << "<irc:" << tag << "> not supported" << endl;
		}
	}
	return "";
}

void IrcResponder::processAlias(const string &cmd) {
	string::size_type index = cmd.find(" ");
	string alias;
	string irc = "";
	if (index == string::npos) {
		alias = toUpper(cmd.substr(1));
		//	commands without params
		if (alias == "QUIT") {
			irc = alias;
		}
		else if (alias == "PART") {
			irc = alias + " :" + mChannel;
		}
	} else {
		alias = toUpper(cmd.substr(1, index));
		string params = cmd.substr(index + 1);
		//	commands with params
		if (alias == "MSG" || alias == "PRIVMSG") {
			index = params.find(" ");
			if (index != string::npos) {
				string nick = params.substr(0, index);
				string msg = params.substr(index + 1);
				irc = "PRIVMSG " + nick + " :" + msg;
			}
		}
		else if (alias == "JOIN" || alias == "PART") {
			//	both have same format so can combine into one
			irc = alias + " :" + params;
		}
	}
	if (!irc.empty()) {
		q.push(irc);
	}
}

void IrcResponder::execute() {
	while (!q.empty()) {
		sendCommand(q.front());
		q.pop();
	}
}

bool IrcResponder::changeNick() {
	if (++nickIndex == config.nicks.size()) {
		return false;
	}
	nick = config.nicks[nickIndex];
	return true;
}

void IrcResponder::recv(string &recv) {
	if (ipt != NULL && ipt->client != NULL) {
		ipt->send(recv);
	}
	if (botControl) {
		//	Since when bot has control, the PassThru server can't track
		//	the nick changes, so the bot doesn't know who it is anymore
	/*	string n = ":" + nick;
		if (recv.find(n) == 0) {
			n = " NICK :";
			if (recv.find(n) != string::npos) {
				string newnick = trim(recv.substr(recv.find(n) + n.length()));
				cerr << "Bot's new nick is: '" << newnick << "'" << endl;
				nick = newnick;
			}
		}
	*/	processLine(recv);
	}
	
}

void IrcResponder::send(const string &send, bool fromIPT) {
	if (sock == NULL) {
		return;
	}
	if (!fromIPT && ipt != NULL && ipt->client != NULL) {
		if (send.find("PRIVMSG ") == 0) {
			ipt->send(":" + nick + " " + send);
		}
	}
	sock->write(send + "\r\n");
}

void IrcResponder::connect() {
	botControl = true;
	gotMotd = true;	//	Hack
	//	Order as recommended in RFC1459
	//	Later, will let this be configured somehow
	//	Perhaps from <bot name="email"/>
	send("PASS " + pass);
	send("NICK " + nick);
	send("USER " + nick + " j-alice.sourceforge.net " + config.host + " :http://j-alice.sourceforge.net");
	
	gotMotd = false;
	
	//	FIXME: This should be sent after recv MOTD as well...
	string channels;
	for (unsigned int ix = 0; ix < config.channels.size(); ++ix) {
		channels += config.channels[ix] + ",";
	}
	channels = channels.substr(0, channels.length() - 1);
	send("JOIN " + channels);
	cout << "Connected to " << config.description << endl;
}

void IrcResponder::disconnect() {
	send("QUIT :bing!");
	sock->disconnect();
	
	if (ipt != NULL) {
		ipt->shutdown("IRC Client disconnected");
		delete ipt;
		ipt = NULL;
	}
	
	SocketHandler::removeSocket(config.description);
	cout << "Disconnected from " << config.description << " (software)" << endl;
}

void IrcResponder::disconnected(const string &msg) {
	SocketHandler::removeSocket(config.description);
	if (!config.autoReconnect || ++retryCount >= (unsigned int)config.maxRetries) {
		cout << "Disconnected from " << config.description << " (" << msg << ")" << endl;
		if (ipt != NULL) {
			ipt->shutdown("IRC Client disconnected");
			delete ipt;
			ipt = NULL;
		}
	} else {
		cout << "Attempting to reconnect to " << config.description << " (" << msg << ")" << endl;
		start(5);
	}
}

void IrcResponder::start(int delay) {
	sock = new Socket(config.host, config.port);
	SocketHandler::addSocket(config.description, sock);
	sock->setListener(new TokenProxyListener(this));
	
	
	timer->addEvent((tNow() + (tSecond() * delay)), new Reconnecter(sock));
}

//	sendCommand(string)

void IrcResponder::sendCommand(const string &command) {
	string::size_type pos = command.find(" ");
	if (pos != string::npos) {
		send(toUpper(command.substr(0, pos)) + command.substr(pos));
	} else {
		send(command);
	}
}

//	processLine(string)
/*
<message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
<prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
<command>  ::= <letter> { <letter> } | <number> <number> <number>
<SPACE>    ::= ' ' { ' ' }
<params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]

<middle>   ::= <Any *non-empty* sequence of octets not including SPACE
               or NUL or CR or LF, the first of which may not be ':'>
<trailing> ::= <Any, possibly *empty*, sequence of octets not including
                 NUL or CR or LF>

<crlf>     ::= CR LF
//
<target>     ::= <to> [ "," <target> ]
<to>         ::= <channel> | <user> '@' <servername> | <nick> | <mask>
<channel>    ::= ('#' | '&') <chstring>
<servername> ::= <host>
<host>       ::= see RFC 952 [DNS:4] for details on allowed hostnames
<nick>       ::= <letter> { <letter> | <number> | <special> }
<mask>       ::= ('#' | '$') <chstring>
<chstring>   ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and
                 comma (',')>

Other parameter syntaxes are:

<user>       ::= <nonwhite> { <nonwhite> }
<letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
<number>     ::= '0' ... '9'
<special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'
<nonwhite>   ::= <any 8bit code except SPACE (0x20), NUL (0x0), CR
                     (0xd), and LF (0xa)>
*/

void IrcResponder::processLine(string &line) {
	string sender, command, params;
	if (line[0] == ':') {
		//	Then we have a PREFIX
		string prefix = line.substr(1);
		string::size_type ix = prefix.find(" ");
		sender = prefix.substr(0, ix);
		sender = sender.substr(0, prefix.find("!"));
		command = prefix.substr(ix + 1, prefix.find(" ", ix + 1) - (ix + 1));
		params = trim(prefix.substr(prefix.find(" ", ix + 1) + 1));
	} else {
		string::size_type ix = line.find(" ");
		command = line.substr(0, ix);
		params = trim(line.substr(ix + 1));
	}
	if (command.empty()) {
		cerr << "Error processing line from server" << endl;
		cerr << line << endl;
		return;
	}
	if (!sender.empty()) {
		mNick = sender;
	}
	//	Now interpret the command, and (possibly) do something :-)
	if (command == "JOIN") {
		//	sender has joined this channel .. so say hello :-)
		//	Use the <pattern>CONNECT</pattern> message for this
		//	What would also like to do is silently set the user's name that the
		//	bot stores to the user's current nick.
		//	Later, can figure out how to use hostmasks to recognise a user.
	} else if (command == "INVITE") {
		//	sender invited me to a channel
		//	first space delimited param is me, 2nd space delimited param is channel
		string::size_type ix = params.rfind(" ");
		string chan = params.substr(ix + 1);
		//	Later, want to spawn a new Responder, so that we know exactly which
		//	channel we are on...
		mChannel = chan;
		send("JOIN " + chan);
	} else if (command == "KICK") {
		//	sender may have kicked me .. join back
		// Later, maybe have it ask why, based on comment?
		//...
	} else if (command == "PRIVMSG") {
		//	received a privmsg (either to a channel I'm on or directly to me)
		//	message follows first occurence of " :"
		//	target is the first space delimited paramater in params
		if (sender == nick) {
			return;
		}	//	Don't want to reply to ourselves, hehe
		
		string logString = "";
		//	Automagically set person's name to their IRC nick, unless
		//	they've given the bot a different name, like:
		//	IRCUser > IRCBot: what is my name?
		//	IRCBot > IRCUser: your name is IRCUser.
		//	IRCUser > IRCBot: my name is Bob
		//	IRCBot > IRCUser: Okay, I'll call you Bob
		//	IRCUser > IrcBot: what is my name?
		//	IRCBot > IRCUser: your name is Bob.
		if (Kernel::predicates->getValue("name", sender).empty()) {
			Kernel::predicates->setValue("name", sender, sender);
		}
		
		string::size_type ix = params.find(" :");
		string target = params.substr(0, ix);
		string message = params.substr(ix + 2);
		if (!target.empty() && (target[0] == '#' || target[0] == '&')) {
			//	The target of the message was a channel
			//	So, now we have to determine if will reply
			mChannel = target;
			if (message.find(nick) == 0) {
				//	Then message is: <nick><message>
				message = trim(message.substr(nick.length()));
				if (message.length() > 0 && !isalpha(message[0])) {
					message = trim(message.substr(1));
				}
				if (!message.empty()) {
					string output = Kernel::respond(message, sender, this);
					StringTokenizer lines(output, "\n");
					logString = sender + " > " + message;
					while (lines.hasMoreTokens()) {
						string next = lines.nextToken();
						logString += "\nbot > ";
						send("PRIVMSG " + target + " :" + sender + ": " + next);
						logString += next;
					}
				}
			} else if (message.find(nick) != string::npos) {
				//	My nick ain't at the start, but it's in the message
				string::size_type minSearch = 0;
				string bot_name = Kernel::respond("BOT NAME", sender, this);
				while ((minSearch = message.find(nick, minSearch)) != string::npos) {
					string m_head = message.substr(0, minSearch);
					string m_tail = message.substr(minSearch + nick.length());
					//	As in submitted patch .. not sure if grammaticaly
					//	correct, but it'll do for moment
					message = m_head + m_tail;
					//	was: message = m_head + bot_name + m_tail;
				}
				if (!message.empty()) {
					StringTokenizer lines(Kernel::respond(message, sender, this), "\n");
					logString = sender + " > " + message;
					while (lines.hasMoreTokens()) {
						string next = lines.nextToken();
						logString += "\nbot > ";
						send("PRIVMSG " + target + " :" + sender + ": " + next);
						logString += next;
					}
				}
			}
		} else {
			//	The target is me :-)
			mChannel = "";
			if (!message.empty()) {
				StringTokenizer lines(Kernel::respond(message, sender, this), "\n");
				logString = sender + " > " + message;
				while (lines.hasMoreTokens()) {
					string next = lines.nextToken();
					logString += "\nbot > ";
					send("PRIVMSG " + sender + " :" + next);
					logString += next;
				}
			}
		}
		if (!logString.empty()) {
			Logger::log(logString, "irc-" + config.host);
		}
		execute();	//	seems the place to do it
	} else if (command == "NOTICE" || command == "ERROR") {
		//	Send to console I suppose
		cout << "\t*** IRC: " << command << " " << params << endl;
	} else if (command == "KILL") {
		//	Check if I got KILLed .. and reconnect
	} else if (command == "PING") {
		//	Need to reply to this!
		send("PONG " + params);
	} else {
		//	Try to convert to a numeric code (3-digit)
		int numeric = atoi(command.c_str());
		//	Put here I suppose
		static int notregisteredcount = 0;
		switch (numeric) {
			case 376:
				//	End of MOTD
				gotMotd = true;
				if (ipt == NULL) {
					ipt = new IrcPassThru(this);
				}
				serverHost = sender;
				return;
			//	Only consider useful commands...
			case 433:
				//	This is where we would change nick .. but can't, so disconnect
				if (!changeNick()) {
					cerr << "*** IRC: Nick is in use .. disconnecting" << endl;
					disconnect();
				} else {
					send("NICK " + nick);
				}
				return;
			case 451:
				//	This is a bitchy problem
				++notregisteredcount;
				if (notregisteredcount > 3) {
					//	No success with registering
					cerr << "*** IRC: Couldn't register .. use a different server" << endl;
					disconnect();
				}
				return;
			case 464:
				cerr << "*** IRC: Password invalid (or not given) .. cannot connect" << endl;
				disconnect();
				return;
			case 465:
				cerr << "*** IRC: Banned from this server .. disconnected" << endl;
				disconnect();
				return;
			default:
				return;	//	Do nothing :-)
		}
	}
}
