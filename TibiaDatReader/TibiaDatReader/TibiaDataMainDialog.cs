using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace TibiaDatReader
{
    public partial class TibiaDataMainDialog : Form
    {
        public TibiaDataMainDialog()
        {
            InitializeComponent();
        }

        private void LoadDatFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog fileDialog = new OpenFileDialog();
            fileDialog.InitialDirectory = "e:\\_diskd\\Program Files\\tibia\\";
            fileDialog.Filter = "Tibia data file|tibia.dat";
            fileDialog.FilterIndex = 1;
            fileDialog.RestoreDirectory = true;
            int total = 0;
            int max = 0;
            StreamWriter outFile = File.CreateText("e:\\projects\\tibiaauto-pub\\mods\\tibiaauto-tiles.xml");
            StreamWriter outFileH = File.CreateText("e:\\projects\\tibiaauto-maps\\mapColors.h");
            outFile.WriteLine("<tiles>");
            outFileH.WriteLine("void setColors() {");
            
            
            if (fileDialog.ShowDialog() == DialogResult.OK)
            {
                Stream datStream = fileDialog.OpenFile();
                if (datStream!=null)
                {
                    int i;
                    // skip first 12 bytes
                    for (i = 0; i < 12; i++)
                        datStream.ReadByte();

                    // read options
                    int currentByte=0;
                    int tileId = 100;
                    bool forceStop = false;
                    do
                    {                        
                        TileData tileData = new TileData();
                        do
                        {
                            int d1, d2, d3, d4;
                            int prevByte = currentByte;                            
                            currentByte = datStream.ReadByte();
                            debugInfo.Items.Add("-> " + currentByte);

                            if (currentByte>6 && currentByte != 0xff) currentByte--;
                            
                            //outFile.Write(" "+currentByte);
                            switch (currentByte)
                            {
                                case 0x0:
                                    // gound tile with speed
                                    tileData.groundtile = true;
                                    d1=tileData.speed = datStream.ReadByte();
                                    debugInfo.Items.Add("X> " + d1);
                                    if (tileData.speed == 0)
                                        tileData.blocking = true;
                                    // ignore next opt byte
                                    d1=datStream.ReadByte();
                                    debugInfo.Items.Add("X> " + d1);
                                    break;                                    
                                    
                                case 0x1:
                                    tileData.moreAlwaysOnTop = true;
                                    // ignore next opt byte
                                    //d1=datStream.ReadByte();
                                    //debugInfo.Items.Add("X> " + d1);
                                    break;
                                     
                                case 0x2:
                                    tileData.alwaysOnTop = true;
                                    break;
                                case 0x3:
                                    // doors, etc.
                                    tileData.canWalkThrough = true;
                                    tileData.alwaysOnTop = true;
                                    break;
                                case 0x4:
                                    tileData.iscontainer = true;
                                    break;
                                case 0x5:
                                    tileData.stackable = true;
                                    break;
                                case 0x6:
                                    // unkonwn ??
                                    break;
                                case 0x7:
                                    tileData.useable = true;
                                    break;
                                case 0x8:
                                    // writtable object
                                    tileData.RWInfo = 3; // r+w
                                    datStream.ReadByte(); // max number of chars
                                    datStream.ReadByte(); // max number of lines
                                    break;
                                case 0x9:
                                    // readable object
                                    tileData.RWInfo = 1; // r
                                    datStream.ReadByte(); // max number of chars
                                    datStream.ReadByte(); // max number of lines
                                    break;
                                case 0xA:
                                    tileData.fluidcontainer = true;
                                    break;
                                case 0xB:
                                    tileData.multitype = true;
                                    break;
                                case 0xC:
                                    tileData.blocking = true;
                                    break;
                                case 0xD:
                                    tileData.notMoveable = true;
                                    break;
                                case 0xE:
                                    tileData.blockingProjectile = true;
                                    break;
                                case 0xF:
                                    // unknown
                                    break;
                                case 0x10:
                                    tileData.pickupable = true;
                                    break;
                                case 0x15:
                                    // make light
                                    datStream.ReadByte(); // number of tiles around
                                    datStream.ReadByte(); // red
                                    datStream.ReadByte(); // green
                                    datStream.ReadByte(); // blue
                                    break;
                                case 0x11:
                                    // see what is under
                                    break;
                                case 0x1E:
                                    // ground tiles that don't cause level change
                                    tileData.noFloorChange = true;
                                    break;
                                case 0x19:
                                    // mostly blocking items, but also items that can pile up in level (boxes, chairs etc)
                                    tileData.blockpickupable = false;
                                    datStream.ReadByte(); // always 8
                                    datStream.ReadByte(); // always 0
                                    break;
                                case 0x14:
                                    // unknown
                                    break;
                                case 0x18:
                                    // unknown
                                    d1=datStream.ReadByte();
                                    d2=datStream.ReadByte();
                                    d3=datStream.ReadByte();
                                    d4=datStream.ReadByte();
                                    debugInfo.Items.Add("UNKNOWN1: " + d1);
                                    debugInfo.Items.Add("UNKNOWN2: " + d2);
                                    debugInfo.Items.Add("UNKNOWN3: " + d3);
                                    debugInfo.Items.Add("UNKNOWN4: " + d4);
                                    break;
                                case 0x1C:
                                    // for minimap drawing
                                    d1=datStream.ReadByte(); // minimap color 1
                                    d2=datStream.ReadByte(); // minimap color 2
                                    outFileH.WriteLine("miniCol1[" + tileId + "]=" + d1+";");
                                    //outFileH.WriteLine("miniCol2[" + tileId + "]=" + d2);                                    
                                    break;
                                case 0x17:
                                    // stairs down
                                    tileData.floorChangeDOWN = true;
                                    break;
                                case 0x1A:
                                    // corpses that don't decay
                                    tileData.canDecay = false;
                                    break;
                                case 0x1B:
                                    // walls
                                    break;
                                case 0x12:
                                    // action possible
                                    break;
                                case 0x13:
                                    // walls 2 types of them same material (total 4 pairs)
                                    break;                                
                                case 0x1D:
                                    // line spot
                                    int optByte = datStream.ReadByte();
                                    debugInfo.Items.Add("o> " + optByte);
                                    // 86 -> openable holes, 77-> can be used to go down, 76 can be used to go up, 82 -> stairs up, 79 switch
                                    switch (optByte)
                                    {
                                        case 0x18:
                                            // new for 7.8                                            
                                            break;
                                        case 0x4C:
                                            // ladders
                                            tileData.floorChangeUP = true;
                                            tileData.requireRightClick = true;
                                            break;
                                        case 0x4D:
                                            // crate - trapdoor
                                            tileData.requireRightClick = true;
                                            break;
                                        case 0x4E:
                                            // rope spot (?)
                                            tileData.requireRope = true;
                                            tileData.floorChangeUP = true;
                                            break;
                                        case 0x4F:
                                            // switch;
                                            break;
                                        case 0x50:
                                            // door
                                            tileData.isDoor = true;
                                            break;
                                        case 0x51:
                                            // door with lock
                                            tileData.isDoorWithLock = true;
                                            break;
                                        case 0x52:
                                            // stairs to up floor
                                            tileData.floorChangeUP = true;
                                            break;
                                        case 0x53:
                                            // mailbox
                                            break;
                                        case 0x54:
                                            // depot
                                            tileData.isDepot = true;
                                            break;
                                        case 0x55:
                                            // trash
                                            break;
                                        case 0x56:
                                            // hole
                                            tileData.floorChangeDOWN = true;
                                            tileData.requireShovel = true;
                                            break;
                                        case 0x57:
                                            // unknown
                                            break;
                                        case 0x58:
                                            // writtable
                                            tileData.RWInfo = 1;
                                            break;                                        
                                       
                                        default:
                                            // ERROR!
                                            debugInfo.Items.Add("ERROR: Unknown extra option met: " + optByte);                                            
                                            break;
                                    } // switch (optByte)
                                    datStream.ReadByte(); // always 4
                                    break;
                                
                                case 0x80:
                                    // new for 7.8                                            
                                    datStream.ReadByte();
                                    break;
                                        
                              


                                case 0xff:
                                    // end of opt list
                                    break;                                    
                                case -1:
                                    // eof
                                    break;
                                default:
                                    // ERROR!
                                    
                                    debugInfo.Items.Add("ERROR: Unknown main option met: " + currentByte);
                                    int ii;
                                    for (ii = 0; ii < 100; ii++)
                                    {
                                        debugInfo.Items.Add("N> " + datStream.ReadByte());
                                    }
                                    
                                    forceStop = true;
                                    break;
                            } // switch (currentByte)
                            //outFile.WriteLine();
                        } while (currentByte != 0xff && currentByte != -1 && !forceStop);

                        if (currentByte == -1)
                            break;
                        // compute extra flags
                        if (tileData.stackable || tileData.multitype || tileData.fluidcontainer)
                            tileData.haveExtraByte = true;
                        if (tileData.alwaysOnTop)
                            tileData.stackPriority = 3;
                        if (tileData.moreAlwaysOnTop)
                            tileData.stackPriority = 4;

                        
                        /* 
                         * IGNORED OPTIONS!
                         * 
                         * ' add special cases of floor changers, for cavebot
                         * Select Case i
                         * ' ramps that change floor when you step in
                         * Case tileID_rampToNorth, tileID_rampToSouth, tileID_rampToRightCycMountain, tileID_rampToLeftCycMountain
                         * DatTiles(i).floorChangeUP = True
    Case tileID_grassCouldBeHole ' grass that will turn into a hole when you step in
      DatTiles(i).floorChangeDOWN = True
    End Select
    
    '[CUSTOM FLAGS FOR BLACKDPROXY]
    'water, for smart autofisher
    If i = tileID_waterWithFish Then
      DatTiles(i).isWater = True
      DatTiles(i).haveFish = True
    End If
    If i = tileID_waterEmpty Then
      DatTiles(i).isWater = True
    End If
    
    ' food, for autoeater
    If i >= tileID_firstFoodTileID And i <= tileID_lastFoodTileID Then
      DatTiles(i).isFood = True
    End If
    If (i >= tileID_firstMushroomTileID) And (i <= tileID_lastMushroomTileID) Then
      DatTiles(i).isFood = True
    End If
    
    ' fields, for a* smart path
    If i >= tileID_firstFieldRangeStart And i <= tileID_firstFieldRangeEnd Then
      DatTiles(i).isField = True
    End If
    If (i >= tileID_secondFieldRangeStart) And (i <= tileID_secondFieldRangeEnd) Then
      DatTiles(i).isField = True
    End If
    Select Case i
    Case tileID_walkableFire1, tileID_walkableFire2, tileID_walkableFire3
      DatTiles(i).isField = False 'dont consider fields that doesnt do any harm
    End Select
    '[/CUSTOM FLAGS FOR BLACKDPROXY]
                         */


                        // special tiles
                        // 1950, 1952 -> ramp up on thais bridge
                        if (tileId==1950||tileId==1952)
                            tileData.floorChangeUP=true;
                        // 1958 -> Stair Case ( lvl 2 bridge rook - probably else where aswell) 
                        if (tileId == 1958)
                            tileData.floorChangeUP = true;
                        // 1960 -> special stairs left-right up
                        if (tileId == 1960)
                            tileData.floorChangeUP = true;
                        // 293 -> "hidden" hole
                        if (tileId == 293)
                            tileData.floorChangeDOWN = true;
                        // 1998 - 2000 -> campfire
                        if (tileId >= 1998 && tileId<=1999)
                            tileData.blocking = true;
                        // 2118 - 2127 -> various fiels (poison, fire, energy)
                        if (tileId >= 2118 && tileId <= 2127)
                            tileData.blocking = true;
                        // stairs in dara
                        if (tileId == 1966)
                            tileData.floorChangeUP = true;
                        // venore stairs
                        if (tileId == 1954 || tileId == 1956)
                            tileData.floorChangeUP = true;
                        // darashia something
                        if (tileId == 867 || tileId == 868)
                            tileData.floorChangeDOWN = true;
                        if (tileId == 867)
                            tileData.requireShovel = true;
                        if (tileId == 867)
                            tileData.canWalkThrough = true;


                        // read graphics
                        int graphWidth = datStream.ReadByte(); 
                        int graphHeight = datStream.ReadByte();
                        if (graphWidth > 1 || graphHeight > 1)
                        {
                            // skip 1 byte
                            datStream.ReadByte();
                        }
                        int graphBlendFrames = datStream.ReadByte();
                        int graphXvid = datStream.ReadByte();
                        int graphYvid = datStream.ReadByte();
                        int graphAnimCount = datStream.ReadByte();
                        int graphLRare = datStream.ReadByte();
                        int graphSize = graphWidth * graphHeight * graphBlendFrames * graphXvid * graphYvid * graphAnimCount * graphLRare;
                        // skip the image
                        String x = "";
                        outFile.Write("<tile ");
                        outParam(outFile, "id", tileId);
                        outParam(outFile, "ground", tileData.groundtile);
                        outParam(outFile, "goUp", tileData.floorChangeUP);
                        outParam(outFile, "goDown", tileData.floorChangeDOWN);
                        outParam(outFile, "blocking", tileData.blocking);
                        outParam(outFile, "speed", tileData.speed);
                        outParam(outFile, "requireRope", tileData.requireRope);
                        outParam(outFile, "requireUse", tileData.requireRightClick);
                        outParam(outFile, "requireShovel", tileData.requireShovel);
                        outParam(outFile, "canWalkThrough", tileData.canWalkThrough);
                        outParam(outFile, "isDepot", tileData.isDepot);
                        outParam(outFile, "isContainer", tileData.iscontainer);
                        outParam(outFile, "notMoveable", tileData.notMoveable);
                        outParam(outFile, "stackable", tileData.stackable||tileData.fluidcontainer);
                        outParam(outFile, "alwaysOnTop", tileData.alwaysOnTop);
                        outParam(outFile, "moreAlwaysOnTop", tileData.moreAlwaysOnTop);
                        //outFile.WriteLine("<tile id=\"" + tileId + "\" ground=\"" + tileData.groundtile + "\" up=\"" + tileData.floorChangeUP + "\"  down=\"" + tileData.floorChangeDOWN + "\" blocking=\"" + tileData.blocking+ "\">");
                        outFile.WriteLine("/>");


                        if (forceStop) graphSize = 0;
                        
                        bool hit = false;
                        for (i = 0; i < graphSize; i++)
                        {
                            int hiByte = datStream.ReadByte();
                            int loByte = datStream.ReadByte();                            
                            int val = loByte * 256 + hiByte;
                            x = x + " " + val;
                            if (val > max)
                                max = val;
                            if (val != 0&&!hit)
                            {
                                hit = true;
                                //File.Copy("d:\\program files\\tibia\\pics\\" + val + ".png", "d:\\program files\\tibia\\items\\" + tileId + ".png");
                            }
                        }
                         

                        String s = "(" + graphWidth + "," + graphHeight + "," + graphBlendFrames + "," + graphXvid + "," + graphXvid + "," + graphAnimCount + "," + graphLRare + ")";

                        debugInfo.Items.Add("INFO: read tile " + tileId + " size=" + graphSize + s);
                        //debugInfo.Items.Add("DEBUG: " + x);                        
                        total += graphSize;                        
                        tileId++;
                    } while (currentByte != -1 && !forceStop);
                    datStream.Close();
                }
            }
            outFile.WriteLine("</tiles>");
            outFile.Close();
            outFileH.WriteLine("};");
            outFileH.Close();
            
            debugInfo.Items.Add("TOTAL = " + total);
            debugInfo.Items.Add("MAX = " + max);
        }

        private void outParam(StreamWriter outFile, string paramName, int paramValue)
        {
            outFile.Write(" " + paramName+"=\"" + paramValue + "\" ");
        }
        private void outParam(StreamWriter outFile, string paramName, bool paramValue)
        {
            outParam(outFile, paramName, paramValue ? 1 : 0);            
        }

      
    }
}