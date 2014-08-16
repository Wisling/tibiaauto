import string
import time

class ModuleFloorSpyr:
      #addresses for Tibia Client 8.10 !
      XRAY_ADDR1 = 0x4C4FBC;
      XRAY_ADDR2 = 0x4C4FC4;
      currX       = 0;
      currY      = 0;
      currZ      = 0;
      SpyAct     = 0;
     
      def getName(self):
          return "Floor Spy"

      def getVersion(self):
          return "1.0"

      def getFunDef(self,nr):
          if (nr==0): return (2,0,self.fun1);
          #if (nr==1): return (0,1000,self.fun2);
          return ();

      def getConfigParam(self,nr):

          return ();

      def fun1(self,params,message):
          selfCh=tareader.readSelfCharacter();
          if(1 == 1):
              if(message == '%ta ld'):
                 tasender.sendTAMessage('Looking down...');
                 offset = -1;
              if(message == '%ta lu'):
                 tasender.sendTAMessage('Looking up...');
                 offset = 1;
              if(message == '%ta lr'):
                 tasender.sendTAMessage('Look reset...');
                 tareader.setMemIntValue(self.XRAY_ADDR1,7);
                 tareader.setMemIntValue(self.XRAY_ADDR2,2);
                 return ();
               
              # player position
              self.currX=selfCh['x'];
              self.currY=selfCh['y'];
              self.currZ=selfCh['z'];
              # code segment hack,
              # groundlevel is for ground and upper levels,
              # original value 7
              groundlevel = tareader.getMemIntValue(self.XRAY_ADDR1);
              # undergroundlevel for underground
              # original value = 2
              undergroundlevel = tareader.getMemIntValue(self.XRAY_ADDR2);
              groundlevel += offset;
              undergroundlevel += offset;
              # character at ground or higher
              # ground = 7. 6->5->4 and so are higher levels
              # 8 -> 9 -> 10 and so are lower levels.
                 
              self.SpyAct     = 1;
              if(self.currZ <= 7):
                 # tasender.sendTAMessage('1');
                 groundlevel = max(groundlevel,self.currZ);
                 tareader.setMemIntValue(self.XRAY_ADDR2,2);   # set undergroundlevel to default
                 tareader.setMemIntValue(self.XRAY_ADDR1,groundlevel);

              if(self.currZ >7):
                  # tasender.sendTAMessage('2');
                  undergroundlevel = max(undergroundlevel,0);
                  undergroundlevel = min(undergroundlevel,7);
                  tareader.setMemIntValue(self.XRAY_ADDR1,7);   # set groundlevel to default
                  tareader.setMemIntValue(self.XRAY_ADDR2,undergroundlevel);

              return ();
             
      def fun2(self,params):
          selfCh=tareader.readSelfCharacter();
          if (self.SpyAct == 1):
              if (self.currX != selfCh['x'] or self.currY != selfCh['y'] or self.currZ != selfCh['z']):
                  self.currX=selfCh['x'];
                  self.currY=selfCh['y'];
                  self.currZ=selfCh['z'];
                  tasender.sendTAMessage('Look reset...');
                  # restore previous view
                  tareader.setMemIntValue(self.XRAY_ADDR1,7);
                  tareader.setMemIntValue(self.XRAY_ADDR2,2);
                  self.SpyAct=0;

tibiaauto.registerPlugin(ModuleFloorSpyr);