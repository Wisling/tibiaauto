#ifndef COMPAT_H
#define COMPAT_H
/** General header, will contain jalice version, platform-specific #defines...etc */
//Funky preprocessor trickyness, now its much neater than it was before
#define SOCK_FLAGS SO_REUSEADDR
#if defined (__BEOS__)
	#undef SOCK_FLAGS
	#define SOCK_FLAGS ( SO_REUSEADDR | SO_NONBLOCK)
	//should this be unsigned int?
	typedef int socklen_t;
#elif defined (WIN32) 
	typedef int FAR socklen_t;
#elif defined (MACOS)	//	c++ -DMACOS ... defines MACOS on commandline
	typedef int socklen_t;
	#define closesocket(x) close(x)
#else//linux & stuff
	//so stupid platforms can hapily use stupid closesocket()
	#define  closesocket(x)  close(x)
#endif

#endif //COMPAT_H
