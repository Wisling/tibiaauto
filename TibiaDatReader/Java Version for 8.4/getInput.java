import java.io.*;

public class getInput {
	GameSprite[] sprite_space = new GameSprite[20000];
	ImageSet[] image_space=new ImageSet[20000];
	int[] FIELDS = new int[]{105,1949,1998,1999,2000,2118,2119,2121,2122,2123,2124,2126,2127,2137,2138,5022,5023,8823,8824,8825,8859};
	int[] FISHING =new int[]{4597,4598,4599,4600,4601,4602};
	int[] SHIFT_ITEMS = new int[]{2853};
	
	boolean ADD_CUSTOM = true;
	//list of stairs not included as "go up"
	int[] ADD_STAIRS = new int[]{856,1950,1952,1954,1956,1958,1960,1962,1964,1966,1977,1978,6909,6911,6913,6915,7542,7544,7546,7548};
	//list of fields to add as blocking
	int[] ADD_FIELDS = new int[]{105,1998,1999,2000,2118,2119,2121,2122,2123,2124,2126,2127,2137,2138,5022,5023};

	int[][] changes = new int[1000][2];
	int numChanges = 0;
	
	int numItems=0;
	int datVersion;
	int item_count;
	int creature_count;
	int effect_count;
	int distance_count;
	int minID;
	int maxID;
	
	boolean PATHFIND_NOFIELDS = false;
	boolean PATHFIND_FIELDS = false;
	boolean SPEED = false;
	boolean PROJECTILE = false;
	boolean BLOCKING = false;
	boolean CONTAINER = false;
	boolean HEIGHT = false;
	boolean BAG_SHIFT = false;
	boolean CYCLE_SPRITE = false;
	boolean PRETTY_COLORS = false;
	boolean ALL_ITEMS_CYCLE = false;
	boolean FISHING_TILES = false;
	

	public int binarySearch(int[] array,int f){
		int a=0;
		int b=array.length-1;
		int m=0;
		while (a<=b){
			m=(int)Math.floor((a+b)/2);
			if (f<array[m]) b = m-1;
			else if (f>array[m]) a = m+1;
			else if (array[m]==f) return m;
		}
		return -1;
	}
	
	public boolean contains(int[] array,int f){
		return binarySearch(array,f)!=-1;
	}

	public getInput(String in,String out){
		//System.out.print(contains(this.FIELDS,105));
		parse(in);
		//edit(out);
	}
	
	public getInput(String[] args){
		if(args.length<1){System.out.println("No input file given");return;}
		if(args.length<2){System.out.println("No output file given");return;}
		if(!(new File(args[0]).exists())) {System.out.println("File "+args[0]+" does not exist.");return;}
		//if(!(new File(args[1]).exists())) {System.out.println("File "+args[1]+" does not exist.");return;}
		String arg = "";
		for (int i=2;i<args.length;i++){
			if (args[i].charAt(0) == '-') arg = args[i];
			if (arg.equals("-s")){
				for (i++;i<args.length && args[i].charAt(0)!='-';i++){
					int t = 0;
					try {t = Integer.parseInt(args[i]);}
					catch (Exception e){System.out.println(args[i]+" bad argument for -s.");}
					switch (t){
						case 1:this.PATHFIND_NOFIELDS=true; break;
						case 2:this.PATHFIND_FIELDS=true; break;
						case 3:this.SPEED=true; break;
						case 4:this.PROJECTILE=true; break;
						case 5:this.BLOCKING=true; break;
						case 6:this.CONTAINER=true; break;
						case 7:this.HEIGHT=true; break;
						case 8:this.BAG_SHIFT=true; break;
						case 9:this.CYCLE_SPRITE=true; break;
						case 10:this.PRETTY_COLORS=true; break;
						case 11:this.ALL_ITEMS_CYCLE=true; break;
						case 12:this.FISHING_TILES=true;break;
					}
				}
				i--;
			} else if(arg.equals("-c")){
				for (i++;i<args.length && args[i].charAt(0)!='-';i++){
					try {this.changes[this.numChanges][0] = Integer.parseInt(args[i]);}
					catch (Exception e){System.out.println(args[i]+" bad argument for -c.");}
					i++;
					if (!(i<args.length && args[i].charAt(0)!='-')) {System.out.println("Not an even number of arguments for -c"); return;}
					try {this.changes[this.numChanges++][1] = Integer.parseInt(args[i]);}
					catch (Exception e){System.out.println(args[i]+" bad argument for -c.");}
				}
				i--;
			} else System.out.println(arg+" is an invalid argument.");
			
			
		}
		parse(args[0]);
		if (this.FISHING_TILES){
			for (int i=0;i<this.FISHING.length;i++){
				changes[this.numChanges][0]=this.FISHING[i];
				changes[this.numChanges++][1]=727;
			}				
		}
		for (int i=0;i<this.numChanges;i++){
			if (changes[i][0]>=this.image_space.length || this.image_space[changes[i][0]]==null){
				System.out.println(changes[i][0]+" out of bounds.");
				return;
			}
			if (changes[i][1]>=this.image_space.length || this.image_space[changes[i][1]]==null){
				System.out.println(changes[i][1]+" out of bounds.");
				return;
			}
			int indo = this.image_space[changes[i][0]].ind;
			int indn = this.image_space[changes[i][1]].ind;
			GameSprite older = this.sprite_space[indo];
			GameSprite newer = this.sprite_space[indn];
			if (older==null){System.out.println("Error with item "+changes[i][0]); return;}
			if (newer==null){System.out.println("Error with item "+changes[i][1]); return;}
			older.hasLight = newer.hasLight;
			older.lightLevel = newer.lightLevel;
			older.lightColor = newer.lightColor;
			older.offsetX = newer.offsetX;
			older.offsetY = newer.offsetY;
			older.offsetZ = newer.offsetZ;
			older.offsetHeightForAll = newer.offsetHeightForAll;
			older.cycleSprite = newer.cycleSprite;
			older.width = newer.width;
			older.height = newer.height;
			older.heightWidth = newer.heightWidth;
			older.frames = newer.frames;
			older.xdiv = newer.xdiv;
			older.ydiv = newer.ydiv;
			older.zdiv = newer.zdiv;
			older.animationLength = newer.animationLength;
			older.numSprites = newer.numSprites;
			this.image_space[changes[i][0]] = new ImageSet(newer.numSprites,indo);
			this.image_space[changes[i][0]].addImage(this.image_space[changes[i][1]].getImageArray());
		}
		
		edit(args[1]);
		System.out.println("Done");
	}

	public int twoc(int a){
		return (a>=0)?a:256+a;
	}
	public int readByte(DataInputStream dis,int n,DataOutputStream dos){
		int ans = readByte(dis,n);
		//writeByte (dos,n,ans); 
		return ans;
	}
	public int readByte(DataInputStream dis,int n){
		int ans=0;
		try {
			for (int i=0;i<n;i++){
				int b = dis.readUnsignedByte();
				//System.out.print(Integer.toHexString(b)+" "+Double.toString(Math.pow(0x100,i))+" ");
				ans+= b*Math.round(Math.pow(0x100,i));
			}
		} catch(IOException e) {
			e.printStackTrace();
		}
		//System.out.print(Integer.toHexString(ans)+",");
		return ans;
	}
	
	public void writeByte(DataOutputStream dos,int n,int info){
		try {
			for (int i=0;i<n;i++){
				byte b = (byte)(info & 0x000000FF);
					dos.writeByte(b);
				//System.out.print(Integer.toHexString(b)+" "+Double.toString(Math.pow(0x100,i))+" ");
				info = Math.round(info/0x100);
			}
		} catch(IOException e) {
			e.printStackTrace();
		}
	}
	
	public void edit(String out){
		//set up output stream
		try{
	    	File fileout= new File(out);
	    	FileOutputStream fos = new FileOutputStream(fileout);
	    	DataOutputStream dos=new DataOutputStream(fos);

	    	writeByte(dos,4,this.datVersion);//0x475D3747
			writeByte(dos,2,this.item_count);
			writeByte(dos,2,this.creature_count);
			writeByte(dos,2,this.effect_count);
			writeByte(dos,2,this.distance_count);

			for (int i=0;i<this.numItems;i++){
				GameSprite sType = this.sprite_space[i];
				if(sType.ground == 1){
					writeByte(dos,1,0x00);
					if (this.SPEED)
						writeByte(dos,1,Math.max(Math.min(70,sType.speed),sType.speed-20));
					else
						writeByte(dos,1,sType.speed);
					writeByte(dos,1,sType.groundType);
				}
				if(sType.alwaysOnTop > 0){
					writeByte(dos,1,sType.alwaysOnTop);
				}
				if(this.CONTAINER)
					writeByte(dos,1,0x04);
				else if(sType.isContainer == 1){
					writeByte(dos,1,0x04);
				}
				if(sType.stackable == 1){
					writeByte(dos,1,0x05);
				}
				if(sType.isUseableImmobile == 1){
					writeByte(dos,1,0x06);
				}
				if(sType.isUseable == 1){
					writeByte(dos,1,0x07);
				}
				if(sType.isRune == 1){
					writeByte(dos,1,0x08);
				}
				if(sType.isReadable == 1){
					if(sType.isWriteable == 1){
						writeByte(dos,1,0x09);
					} else {
						writeByte(dos,1,0x0A);
					}
					writeByte(dos,2,sType.textLength);
				}
				if(sType.isFluid == 1){
					writeByte(dos,1,0x0B);
				}
				if(sType.isSplash == 1){
					writeByte(dos,1,0x0C);
				}
				if(sType.blocking == 1){
					if(this.BLOCKING){}
					else
						writeByte(dos,1,0x0D);
				}
				if(sType.notMoveable == 1){
					writeByte(dos,1,0x0E);
				}
				if(sType.blockProjectile == 1){
					if (this.PROJECTILE)
					{} else
						writeByte(dos,1,0x0F);
				}
				if(sType.blockPathFind == 1){
					if (this.PATHFIND_NOFIELDS){
						if (sType.blocking==1 || sType.goDown==1 || sType.goUp==1 || sType.alwaysOnTop==1 || contains(this.FIELDS,sType.id)){
							writeByte(dos,1,0x10);
						}
					} else if(this.PATHFIND_FIELDS){
						if (sType.blocking==1 || sType.goDown==1 || sType.goUp==1 || sType.alwaysOnTop==1){
							writeByte(dos,1,0x10);
						}
					} else
						writeByte(dos,1,0x10);
				}
				if(sType.pickupable == 1){
					writeByte(dos,1,0x11);
				}
				if(sType.isHangable == 1){
					writeByte(dos,1,0x12);
				}
				if(sType.isHorizontal == 1){
					writeByte(dos,1,0x13);
				}
				if(sType.isVertical == 1){
					writeByte(dos,1,0x14);
				}
				if(sType.rotateable == 1){
					writeByte(dos,1,0x15);
				}
				if(sType.hasLight == 1){
					writeByte(dos,1,0x16);
					writeByte(dos,2,sType.lightLevel);
					writeByte(dos,2,sType.lightColor);
				}
				if(sType.floorAlwaysVisible == 1){
					writeByte(dos,1,0x17);
				}
				if(sType.isHole == 1){
					writeByte(dos,1,0x18);
				}
				if (this.BAG_SHIFT && contains(this.SHIFT_ITEMS,sType.id)){
					writeByte(dos,1,0x19);
					writeByte(dos,2,6);
					writeByte(dos,2,6);					
				}else if(sType.offsetX!=0 || sType.offsetY!=0){
					writeByte(dos,1,0x19);
					writeByte(dos,2,sType.offsetX);
					writeByte(dos,2,sType.offsetY);
				}
				if (this.HEIGHT && sType.offsetZ == 0 && sType.notMoveable!=1){
					writeByte(dos,1,0x1A);
					writeByte(dos,2,1);
				} else if(sType.offsetZ!=0){
					writeByte(dos,1,0x1A);
					writeByte(dos,2,sType.offsetZ);
				}
				if(sType.offsetHeightForAll == 1){
					writeByte(dos,1,0x1B);
				}
				if (this.CYCLE_SPRITE){
					writeByte(dos,1,0x1C);
				}else if(sType.cycleSprite == 1){
					writeByte(dos,1,0x1C);
				}
				if (this.PRETTY_COLORS){
					writeByte(dos,1,0x1D);
					writeByte(dos,2,(int)Math.round(Math.random()*256));
				} else if(sType.hasColour == 1){
					writeByte(dos,1,0x1D);
					writeByte(dos,2,sType.minimapColor);
				}
				if(sType.actionType!=0){
					writeByte(dos,1,0x1E);
					writeByte(dos,1,sType.actionType);
					writeByte(dos,1,sType.actionVersion);
				}
				if(sType.hasNoWhitespace == 1){
					writeByte(dos,1,0x1F);
				}
				writeByte(dos,1,0xFF);
				
				if(this.ALL_ITEMS_CYCLE && sType.id<this.item_count){
					writeByte(dos,1,sType.width);
					writeByte(dos,1,sType.height);
					if((sType.width > 1) || (sType.height > 1)){
						writeByte(dos,1,sType.heightWidth);
					}
					writeByte(dos,1,sType.frames);
					writeByte(dos,1,1);
					writeByte(dos,1,1);
					writeByte(dos,1,1);
					writeByte(dos,1,sType.animationLength*sType.ydiv*sType.zdiv*sType.xdiv);
					sType.numSprites=sType.width*sType.height*sType.frames*sType.xdiv*sType.ydiv*sType.zdiv*sType.animationLength;
				} else {
					writeByte(dos,1,sType.width);
					writeByte(dos,1,sType.height);
					if((sType.width > 1) || (sType.height > 1)){
						writeByte(dos,1,sType.heightWidth);
					}
					writeByte(dos,1,sType.frames);
					writeByte(dos,1,sType.xdiv);
					writeByte(dos,1,sType.ydiv);
					writeByte(dos,1,sType.zdiv);
					writeByte(dos,1,sType.animationLength);
				}
				// Write the sprite ids
				int[] images = this.image_space[sType.id].getImageArray();
				for(int j = 0; j < sType.numSprites; j++) {
					writeByte(dos,2,images[j]);
				}
				dos.flush();
			}
			
		    fos.close();
		    dos.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public void parse(String in){
		int ITEM_GROUP_CONTAINER = 1;
		int ITEM_GROUP_RUNE = 2;
		int ITEM_GROUP_WRITEABLE = 3;
		int ITEM_GROUP_FLUID = 4;
		int ITEM_GROUP_SPLASH = 5;
		//set up input stream for 'in'
		File filein = new File(in);
		FileInputStream fis = null;
		BufferedInputStream bis = null;
		DataInputStream dis = null;

    	try {
			fis = new FileInputStream(filein);
			bis = new BufferedInputStream(fis);
			dis = new DataInputStream(bis);
			
			//set up output stream
	    	this.datVersion = readByte(dis,4);
			this.item_count = readByte(dis,2);
			this.creature_count = readByte(dis,2);
			this.effect_count = readByte(dis,2);
			this.distance_count = readByte(dis,2);
						
			this.minID = 100;
			this.maxID = item_count + creature_count + effect_count + distance_count;
			for (int id=minID; id<=maxID;id++){
				GameSprite sType = new GameSprite();
				this.sprite_space[this.numItems++] = sType;
				sType.id = id;				
				int optbyte;
				do {
					optbyte = readByte(dis,1);
					switch(optbyte){
						case 0x00: //is groundtile
							sType.ground = 1;
							sType.speed = readByte(dis,1);// speed modifier
							sType.groundType = readByte(dis,1);//ground type
							break;
						case 0x01: //Mostly ground tiles
							sType.alwaysOnTop = 1;
							sType.moreAlwaysOnTop = 1;
							//sType->alwaysOnTopOrder = 1;
							break;
						case 0x02: //Mostly object tiles(trees, coffins, wall decos) and walls
							sType.alwaysOnTop = 1;
							sType.moreAlwaysOnTop = 2;
							//sType->alwaysOnTopOrder = 2;
							break;
						case 0x03: //Anything you can walk under.  Cave overhangs. 
							sType.alwaysOnTop = 1;
							sType.moreAlwaysOnTop = 3;
							//sType->alwaysOnTop = true;
							//sType->alwaysOnTopOrder = 3;
							break;
						case 0x04: //is a container
							sType.isContainer = 1;
							sType.group = ITEM_GROUP_CONTAINER;
							break;
						case 0x05: //is stackable
							sType.stackable = 1;
							break;
						case 0x06: //immobile and require use(ladders, fresh corpses)
							sType.isUseableImmobile = 1;
							sType.requireUse = 1;
							break;
						case 0x07: //is useable
							sType.isUseable = 1;
							sType.requireUse = 1;
							break;
						case 0x08: //runes
							sType.isRune = 1;
							sType.group = ITEM_GROUP_RUNE;
							break;
						case 0x09: //writeable objects
							sType.isReadable = 1;
							sType.isWriteable = 1;
							sType.group = ITEM_GROUP_WRITEABLE;
							//sType->readable = true;
							sType.textLength = readByte(dis,2);
							//file.getU16(); //maximum text length?
							break;
						case 0x0A: //writeable objects that can't be edited
							sType.isReadable = 1;
							sType.isWriteable = 0;
							//sType->readable = true;
							sType.textLength = readByte(dis,2);
							//file.getU16(); //maximum text length?
							break;
						case 0x0B: //can contain fluids
							sType.isFluid = 1;
							sType.group = ITEM_GROUP_FLUID;
							break;
						case 0x0C: //liquid with states
							sType.isSplash = 1;
							sType.group = ITEM_GROUP_SPLASH;
							break;
						case 0x0D: //is blocking
							sType.blocking = 1;
							break;
						case 0x0E: //is not moveable
							sType.notMoveable = 1;
							break;
						case 0x0F: //blocks missiles (walls, magic wall etc)
							sType.blockProjectile = 1;
							break;
						case 0x10: //blocks monster movement (flowers, parcels etc)
							sType.blockPathFind = 1;
							break;
						case 0x11: //can be equipped
							sType.pickupable = 1;
							break;
						case 0x12: //wall items
							sType.isHangable = 1;
							break;
						case 0x13:
							sType.isHorizontal = 1;
							break;
						case 0x14:
							sType.isVertical = 1;
							break;
						case 0x15: //rotateable items
							sType.rotateable = 1;
							break;
						case 0x16: //light info ..
							sType.hasLight = 1;
							sType.lightLevel = readByte(dis,2);//level
							sType.lightColor = readByte(dis,2);//colour
							//sType->lightColor = lightcolor;
							break;
						case 0x17: // Tile changes floor view. No object has this...
							sType.floorAlwaysVisible=1;
							break;
						case 0x18: //floor change down
							sType.isHole = 1;
							sType.goDown = 1;
							break;
						case 0x19: { //Draw offset
							sType.offsetX = readByte(dis,2);//x offset
							sType.offsetY = readByte(dis,2);//y offset
						} break;
						case 0x1A: {
							sType.offsetZ = readByte(dis,2);//height offset
						} break;
						case 0x1B://draw with height offset for all parts (2x2) of the sprite
							sType.offsetHeightForAll = 1;
							break;
						case 0x1C: // sprite cycles even when still
							sType.cycleSprite = 1;
							break;
						case 0x1D: { // Minimap color
							sType.hasColour = 1;
							sType.minimapColor = readByte(dis,2);
						} break;
						case 0x1E:  {// Action items
							int type = readByte(dis,1);
							sType.actionType = type;
							sType.actionVersion = readByte(dis,1);
							
							switch (type){
								case 76: //ladder
									sType.goUp=1; 
									sType.requireUse=1;
									break;
								case 77: //grate
									sType.goDown=1;
									sType.requireUse=1;
									break;
								case 78://ropespot
									sType.goUp=1; 
									sType.requireRope=1;
									break;
								case 79://Switch
									break;
								case 80://Door
									break;
								case 81://Lockable door
									break;
								case 82://Stairs
									sType.goUp=1; 
									break;
								case 83://Mailbox
									break;
								case 84://depot box
									sType.isDepot=1;
									break;
								case 85://Garbage Bin
									break;
								case 86://Closed hole 
									sType.goDown=1;
									sType.requireShovel=1; 
									break;
								case 87://Readable Sign
									break;
								case 88://Readable Paperwork
									break;	
							}
						} break;
						case 0x1F://Tile that shows lower floor(holes, borders on roofs)
							sType.hasNoWhitespace = 1;
							break;
						case 0xFF:
							break;
						default: {
							System.out.println("Tibia.dat: Unknown optbyte '"+Integer.toString(optbyte)+","+Integer.toString(id));
							return;
						} //break;
					}
					sType.canWalkThrough = (sType.blocking==0 && sType.ground==1)?1:0;
				} while (optbyte != 0xFF);

				sType.width=readByte(dis,1);
				sType.height=readByte(dis,1);
				if((sType.width > 1) || (sType.height > 1)){
					sType.heightWidth = readByte(dis,1); // No idea what this is....
				}

				sType.frames=readByte(dis,1); // Number of blendframes (some sprites consist of several merged sprites)
				sType.xdiv=readByte(dis,1);
				sType.ydiv=readByte(dis,1);
				sType.zdiv=readByte(dis,1); // Is this ever used? Maybe it means something else?
				sType.animationLength=readByte(dis,1); // Length of animation

				sType.numSprites = sType.width * sType.height * sType.frames * 
					sType.xdiv * sType.ydiv * sType.zdiv * sType.animationLength;
				// Read the sprite ids
				ImageSet img = new ImageSet(sType.numSprites,this.numItems-1);
				for(int i = 0; i < sType.numSprites; i++) {
					int sprite_id;
					sprite_id=readByte(dis,2);
					if(this.image_space[id] == null) {
						img.addImage(sprite_id);
					}
				}
				this.image_space[id]=img;
				
				// Add customized properties
				if (this.ADD_CUSTOM){
					if (contains(this.ADD_STAIRS,id)) sType.goUp = 1;
					if (contains(this.ADD_FIELDS,id)) sType.blocking = 1;	
				}
			}
			System.out.println(this.sprite_space[62]);
			if(this.ADD_CUSTOM){
				File fileout= new File("tiles.xml");
		    	FileOutputStream fos = new FileOutputStream(fileout);
		    	DataOutputStream dos=new DataOutputStream(fos);
		    	dos.writeBytes("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");
				dos.write(new byte[]{0x0D,0x0A});
		    	dos.writeBytes("<tiles>");
				dos.write(new byte[]{0x0D,0x0A});
				int end = this.numItems;
				for (int i=0;i<end;i++){
					dos.writeBytes("\t"+this.sprite_space[i].toString());
					dos.write(new byte[]{0x0D,0x0A});
				}
				dos.writeBytes("</tiles>");
	
				dis.close();
				dos.close();
			}
			
			fis.close();
			bis.close();
			dis.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}catch (Exception e) {
			System.out.println("Problem reading in .dat file. It may be corrupt.");
		}
	}
}
