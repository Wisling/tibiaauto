public class GameSprite {
	int id=0;
	int ground=0;
	int speed=0;
	int groundType=0;
	int moreAlwaysOnTop=0;
	int isContainer=0;
	int group=0;
	int stackable=0;
	int isUseableImmobile=0;
	int isUseable=0;
	int isRune=0;
	int isReadable=0;
	int isWriteable=0;
	int textLength=0;
	int isFluid=0;
	int isSplash=0;
	int blocking=0;
	int notMoveable=0;
	int blockProjectile=0;
	int blockPathFind=0;
	int pickupable=0;
	int isHangable=0;
	int isHorizontal=0;
	int isVertical=0;
	int rotateable=0;
	int hasLight=0;
	int lightLevel=0;
	int lightColor=0;
	int floorAlwaysVisible=0;
	int isHole=0;
	int offsetX=0;
	int offsetY=0;
	int offsetZ=0;
	int offsetHeightForAll=0;
	int cycleSprite=0;
	int hasColour=0;
	int minimapColor=0;
	int actionType=0;
	int actionVersion=0;
	int hasNoWhitespace=0;
	int width=0;
	int height=0;
	int heightWidth=0;
	int frames=0;
	int xdiv=0;
	int ydiv=0;
	int zdiv=0;
	int animationLength=0;
	int numSprites=0;
	int alwaysOnTop=0;
	int canWalkThrough=0;
	int goUp=0;
	int goDown=0;
	int isDepot=0;
	int requireShovel=0;
	int requireRope=0;
	int requireUse=0;

	public GameSprite(){
	}
	public String toTable(){
		return this.id+","+ 
		this.ground+","+
		this.speed+","+
		this.groundType+","+
		this.moreAlwaysOnTop+","+
		this.isContainer+","+
		this.group+","+
		this.stackable+","+
		this.isUseableImmobile+","+
		this.isUseable+","+
		this.isRune+","+
		this.isReadable+","+
		this.isWriteable+","+
		this.textLength+","+
		this.isFluid+","+
		this.isSplash+","+
		this.blocking+","+
		this.notMoveable+","+
		this.blockProjectile+","+
		this.blockPathFind+","+
		this.pickupable+","+
		this.isHangable+","+
		this.isHorizontal+","+
		this.isVertical+","+
		this.rotateable+","+
		this.hasLight+","+
		this.lightLevel+","+
		this.lightColor+","+
		this.floorAlwaysVisible+","+
		this.isHole+","+
		this.offsetX+","+
		this.offsetY+","+
		this.offsetZ+","+
		this.offsetHeightForAll+","+
		this.cycleSprite+","+
		this.hasColour+","+
		this.minimapColor+","+
		this.actionType+","+
		this.actionVersion+","+
		this.hasNoWhitespace+","+
		this.width+","+
		this.height+","+
		this.heightWidth+","+
		this.frames+","+
		this.xdiv+","+
		this.ydiv+","+
		this.zdiv+","+
		this.animationLength+","+
		this.numSprites+","+
		this.alwaysOnTop+","+
		this.canWalkThrough+","+
		this.goUp+","+
		this.goDown+","+
		this.isDepot+","+
		this.requireShovel+","+
		this.requireRope+","+
		this.requireUse;

	}
		
	public String toXML(){
		String s = "<tile alwaysOnTop=\""+this.alwaysOnTop
		+"\" blocking=\""+this.blocking
		+"\" canWalkThrough=\""+this.canWalkThrough
		+"\" goDown=\""+this.goDown
		+"\" goUp=\""+this.goUp
		+"\" ground=\""+this.ground
		+"\" id=\""+this.id
		+"\" isContainer=\""+this.isContainer
		+"\" isDepot=\""+this.isDepot
		+"\" moreAlwaysOnTop=\""+this.moreAlwaysOnTop
		+"\" notMoveable=\""+this.notMoveable
		+"\" requireRope=\""+this.requireRope
		+"\" requireShovel=\""+this.requireShovel
		+"\" requireUse=\""+this.requireUse
		+"\" speed=\""+this.speed
		+"\" stackable=\""+this.stackable
		+"\"/>";
		
		return s;
	}
	public String toString(){
		return toXML();
	}
}