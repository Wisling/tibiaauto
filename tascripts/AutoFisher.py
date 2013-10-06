import random

class ModuleAutoFisher:
      def getName(self):
          return "Auto fisher"

      def getVersion(self):
          return "1.0"

      def getFunDef(self,nr):
          if (nr==0): return (0,500,self.fun1);
          return ();

      def getConfigParam(self,nr):
          if (nr==0): return ('fishOnlyWhenCap','Fish only when more than 5 capacity is left');
          if (nr==1): return ('moveFromHand','Move fish from hands to containers');
          return ();

      def fun1(self,params):
          selfCh=tareader.readSelfCharacter();
          if (params['moveFromHand']):
             fishId=taitem.getValueForConst('fish');
             # left hand
             itemLeftHand=tareader.readItem(taitem.getValueForConst('addrLeftHand')+taitem.getValueForConst('addrOffset'));
             if (itemLeftHand['quantity']==0): itemLeftHand['quantity']=1;
             if (itemLeftHand['objectId']==fishId):
                for contNr in range(taitem.getValueForConst('maxContainers')):
                    cont = tareader.readContainer(contNr);
                    if (cont['flagOnOff'] and cont['itemsInside']<cont['size']):
                       tasender.moveObjectBetweenContainers(itemLeftHand['objectId'],0x06,0,0x40+contNr,0,itemLeftHand['quantity'])
                       break;
             # right hand
             itemRightHand=tareader.readItem(taitem.getValueForConst('addrRightHand')+taitem.getValueForConst('addrOffset'));
             if (itemRightHand['quantity']==0): itemRightHand['quantity']=1;
             if (itemRightHand['objectId']==fishId):
                for contNr in range(taitem.getValueForConst('maxContainers')):
                    cont = tareader.readContainer(contNr);
                    if (cont['flagOnOff'] and cont['itemsInside']<cont['size']):
                       tasender.moveObjectBetweenContainers(itemRightHand['objectId'],0x06,0,0x40+contNr,0,itemRightHand['quantity'])
                       break;
                   
          # capacity limit check
          if (params['fishOnlyWhenCap']):
             if (selfCh['cap']<6):
                return;

          # lookup for a suitable fishy tile
          offsetX=offsetY=0;
          for loopNr in range(20):
              offsetX=random.randint(-7,7);
              offsetY=random.randint(-5,5);
              tileId = tareader.mapGetPointItemId(offsetX,offsetY,0,0);
              if (tileId>=taitem.getValueForConst('waterWithFishStart') and tileId <=taitem.getValueForConst('waterWithFishEnd')):
                 break;

          # if a suitable tile is found then just try to fish a fish
          if (offsetX or offsetY):
             rodObjectId=taitem.getValueForConst('fishingRod');
             fishingRodCont=0x0a;
             fishingRodPos=0;

             fishingRodItem=tautil().lookupItem(rodObjectId);
             if (fishingRodItem['contNr']!=-1):
                fishingRodCont=0x40+fishingRodItem['contNr'];
                fishingRodPos=fishingRodItem['pos'];


             tileId=tareader.mapGetPointItemId(offsetX,offsetY,0,0);
             tasender.useWithObjectFromContainerOnFloor(rodObjectId,fishingRodCont,fishingRodPos,tileId,selfCh['x']+offsetX,selfCh['y']+offsetY,selfCh['z']);





tibiaauto.registerPlugin(ModuleAutoFisher)