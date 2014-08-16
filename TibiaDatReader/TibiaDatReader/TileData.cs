using System;
using System.Collections.Generic;
using System.Text;

namespace TibiaDatReader
{
    class TileData
    {
        public TileData()
        {
            RWInfo = 0;
            blockpickupable = true;
            speed = 0;
            canDecay = true;
            stackPriority = 1;
        }
        public bool iscontainer;
        public int RWInfo;
        public bool fluidcontainer;
        public bool stackable;
        public bool multitype;
        public bool useable;
        public bool notMoveable;
        public bool alwaysOnTop;
        public bool groundtile;
        public bool blocking;
        public bool blockpickupable;
        public bool pickupable;
        public bool blockingProjectile;
        public bool canWalkThrough;
        public bool noFloorChange;
        public bool isDoor;
        public bool isDoorWithLock;
        public int speed;
        public bool canDecay;
        public bool haveExtraByte;        
        public int stackPriority;        
        public bool floorChangeUP;
        public bool floorChangeDOWN;
        public bool requireRightClick;
        public bool requireRope;
        public bool requireShovel;        
        //public bool isField;
        public bool isDepot;
        public bool moreAlwaysOnTop;

    }
}
