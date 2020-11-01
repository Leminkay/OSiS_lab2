
int delta(int x, int y){
	if (x > y){
		int t = y;
		y = x;
		x = t;
	}
	int mFive = -1;
	int mSeven = -1;
	for(int i = x; i <=y; i++){
		if(i % 5 == 0){
			mFive = i;
			break;
		}
	}
	for(int i = y; i >=x; i--){
		if(i % 7 == 0){
			mSeven = i;
			break;
		}
	}
	if(mFive == -1 || mSeven == -1){
		return 0;
	}
	return mSeven - mFive;
}
