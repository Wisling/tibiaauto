class tautil:
      """
      Tibia Auto various utily class
      """
      def lookupItem(self,itemObjectId):
         """
         Lookup itemObjectId in any open container.
         """
         for contNr in range(taitem.getValueForConst('maxContainers')):
             cont=tareader.readContainer(contNr);
             for itemNr in range(cont['itemsInside']):
                 item=tareader.readContainerItem(contNr,itemNr);
                 if (cont['flagOnOff'] and item['objectId']==itemObjectId):
                    item['contNr']=contNr;
                    return item;
         return {'contNr':-1};