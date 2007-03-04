import string
import time

class ModuleRingChanger:
      def getName(self):
          return "RingChanger"

      def getVersion(self):
          return "1.0"

      def getFunDef(self,nr):
          if (nr==0): return (0,60000,self.ringChanger);
          return ();

      def getConfigParam(self,nr):
          if (nr==0): return ('ringName','name of ring to change (default: Life Ring)');
          return ();

      def ringChanger(self,params):
          itemSlotRing=tareader.readItem(taitem.getValueForConst('addrSlotRing')+taitem.getValueForConst('addrOffset'));
          if(itemSlotRing['objectId']==0):
              ringId = taitem.getObjectId('Life Ring');
              ringCont=0x0a;
              ringPos=0;

              ringItem=tautil().lookupItem(ringId);
              if (ringItem['contNr']!=-1):
                ringCont=0x40+ringItem['contNr'];
                ringPos=ringItem['pos'];
                tasender.moveObjectBetweenContainers(ringId,ringCont,ringPos,0x09,0,1);

          

tibiaauto.registerPlugin(ModuleRingChanger)

