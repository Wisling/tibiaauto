import winsound

class ModuleKickAndStuckWarning:
    LastPos = (0,0);
    IS_INGAME = 0x75D420;

    def getName(self):
        selfCh=tareader.readSelfCharacter();
        self.LastPos=(selfCh['x'],selfCh['y']);
        return "KickAndStuckWarning"

    def getVersion(self):
        return "1.0"

    def getFunDef(self,nr):
        if (nr==0): return (0,1000*30,self.StuckWarningFunction);
        if (nr==1): return (0,1000*10,self.KickWarningFunction);
        return ();

    def getConfigParam(self,nr):
        if (nr==0): return ('useStuckWarning','use Stuck warning (default: 0, to enable enter 1)');
        if (nr==1): return ('useKickWarning','use Kick warning (default: 1, to disable enter 0)');
        return ();

    def StuckWarningFunction(self,params):
        if (params['useStuckWarning'] == '1'):
            selfCh=tareader.readSelfCharacter();
            Pos1=(selfCh['x'],selfCh['y']);
            if (Pos1 == self.LastPos):
                winsound.Beep(500, 1000);
                winsound.Beep(200, 100);
                winsound.Beep(500, 1000);

            self.LastPos = Pos1;

        return();

    def KickWarningFunction(self,params):
        if (params['useKickWarning'] == '0'):
            return();

        stillOnline = tareader.getMemIntValue(self.IS_INGAME);
        if(stillOnline != 8):
            winsound.Beep(500, 1000);
            winsound.Beep(200, 100);
            winsound.Beep(500, 1000);


tibiaauto.registerPlugin(ModuleKickAndStuckWarning)

