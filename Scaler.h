#ifndef SCALER_H
#define SCALER_H

#define DX(x) (scaler->dx(x))
#define DY(y) (scaler->dy(y))

#define HEIGHT (scaler->height())
#define WIDTH (scaler->width())

#define Y56 (scaler->gY56())
#define X56 (scaler->gX56())

class Scaler {

private:
	int windowWidth;
	int windowHeight;
	int cWidth;
	int cHeight;
	int x56;
	int y56;

public:
	Scaler();
	static void init();

	inline int dx(int x) const { return (int) (cWidth*x); }
	inline int dy(int y) const { return (int) (cHeight*y); }

	inline int width() const { return windowWidth; }
	inline int height() const { return windowHeight; }

	inline int gX56() const { return x56; }
	inline int gY56() const { return y56; }

private:
	void rescale(int width, int height);
};

extern Scaler* scaler;

#endif