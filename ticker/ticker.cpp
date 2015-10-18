typedef int (*pfunc)(bool);

class Ticker {
private:
	 long next;
	 int freq;
	 pfunc CB;
	 
	 
 public:
	// void bool flag() CB;
	 void tick(long t)
	 {
		 if (t > next) {
			// CB();
			 next += freq;		 
	 }
}
Ticker(long nxt,int f, pfunc tCB);
};

Ticker::Ticker(long nxt, int f, pfunc tCB ){
 this->next = next -f;
 this->freq = f;
 this->CB = tCB;
}

int fred(bool f){
	if (f){
		return 1;
	};
	return 0;
}

int main(){

	Ticker T(1000,10,fred);
	T.tick(1001);
	

}