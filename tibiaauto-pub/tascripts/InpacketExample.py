class ModuleInpacketExample:# MUST HAVE MAP RESEARCHED!!!!
    def getName(self):
        return "InpacketExample"

    def getVersion(self):
        return "1.00"

    def getFunDef(self,nr):
        #Precise documentation on the tre.lib regex package is no longer available, but the
        # rules are similar to if not the same as those found at http://docs.python.org/library/re.html
        # you may try the archived pages of laurikari.net/tre/ at http://web.archive.org/web/20110723181408/http://laurikari.net/tre/documentation/regex-syntax/
        #Example of type 3 function
        #Bad regex's give an error in Tibia when the script is loaded.
        if (nr==0): return (3,".*",self.dosomethingwithpacket); #matches all packets
        #if (nr==0): return (3,"^(\x6b|\x6d)",self.dosomethingwithpacket); #matches packets starting with 0x6B or 0x6D
        #if (nr==0): return (3,"^\xaa" + r".*\^_\^.*",self.dosomethingwithpacket); #matches someone saying something containing ^_^
        return ();

    def getConfigParam(self,nr):
        return ();

    def dosomethingwithpacket(self,bytestring):
        print repr(bytestring);
        tasender.sendTAMessage('%02x ' % ord(bytestring[0]) + "".join(map(lambda x:(32<=ord(x)<128) and (x) or ('%02x ' % ord(x)),bytestring[1:min(100,len(bytestring))]))); #print readable characters
        #tasender.sendTAMessage(" ".join(map(lambda x:'%02x' % ord(x),bytestring[1:min(100,len(bytestring))]))); # Hex only
        if len(bytestring) >= 40 and bytestring[39] == 'A':
            #do stuff
            pass
        return;
tibiaauto.registerPlugin(ModuleInpacketExample);
