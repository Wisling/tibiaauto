class ModuleShowInvisibles:
      BATTLELIST_BEGIN = 0x605A70;
      BATTLELIST_END = 0x60B830;
      STEP_CREATURE = 160;
      OFFSET_CREATURE_TYPE = 3;
      CREATURE_TYPE_NPC = 64;
      OFFSET_CREATURE_OUTFIT = 0x60;
      OUTFIT_INVISIBLE = 0;
      OUTFIT_PIG = 60;
      
      def getName(self):
          return "ShowInvisibles"

      def getVersion(self):
          return "1.0"

      def getFunDef(self,nr):
          if (nr==0): return (0,1000,self.showinvisibles);
          return ();

      def getConfigParam(self,nr):
          return ();

      def showinvisibles(self,params):
                             
          for x in range(self.BATTLELIST_BEGIN, self.BATTLELIST_END, self.STEP_CREATURE):
              alienType = tareader.getMemIntValue(x+self.OFFSET_CREATURE_TYPE);
              if (alienType & self.CREATURE_TYPE_NPC == self.CREATURE_TYPE_NPC):
                  alienOutfit = tareader.getMemIntValue(x+self.OFFSET_CREATURE_OUTFIT);
                  if (alienOutfit == self.OUTFIT_INVISIBLE):
                      tareader.setMemIntValue(x+self.OFFSET_CREATURE_OUTFIT, self.OUTFIT_PIG);
                      
                      

      

tibiaauto.registerPlugin(ModuleShowInvisibles)

