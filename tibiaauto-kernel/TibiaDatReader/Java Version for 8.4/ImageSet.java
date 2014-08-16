public class ImageSet {
	int ind;
	int[] images;
	int numImages;
	public ImageSet(int n,int i){
		this.ind=i;
		this.numImages=0;
		this.images = new int[n];
	}
	
	public void addImage(int n){
		this.images[this.numImages++] = n;
	}

	public void addImage(int[] n){
		for(int i=0;i<n.length;i++)
			this.addImage(n[i]);
	}
	
	public int[] getImageArray(){
		int[] tmp = new int[this.numImages];
		for (int i=0;i<this.numImages;i++) tmp[i] = this.images[i];
		return tmp;
	}
	public String toString(){
		String s = "";
		for (int i = 0;i<1;i++)//this.images.length;i++)
			s += this.images[i]+","; 
		return s.substring(s.length()-1);
	}
	
}
