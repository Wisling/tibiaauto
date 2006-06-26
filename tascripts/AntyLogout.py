class ModuleAntyLogout:
      def getName(self):
          return "Anty logout"

      def getVersion(self):
          return "1.0"

      def getFunDef(self,nr):
          if (nr==0): return (0,1000*180,self.fun1);
          return ();
          
      def getConfigParam(self,nr):
          return ();

      def fun1(self,params):
          selfCh=tareader.readSelfCharacter();
          lookDir=selfCh['lookDirection'];
          if (lookDir==0): tasender.turnDown();tasender.turnUp();
          if (lookDir==1): tasender.turnLeft(); tasender.turnRight();
          if (lookDir==2): tasender.turnUp(); tasender.turnDown();
          if (lookDir==3): tasender.turnRight(); tasender.turnLeft();


tibiaauto.registerPlugin(ModuleAntyLogout)