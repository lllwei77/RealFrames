#ifndef H_FRAME_DATA
#define H_FRAME_DATA

#include "stdlib.h"


class FrameData
{
public:
	FrameData() {
		_data = NULL;
		_size = 0;
	}

	FrameData(char *data, int len) {
		_data = data;
		_size = len;
	}

	virtual ~FrameData() {
		if (_data) free(_data);
	}

public:
	void setData(char *data, int size) {
		_data = data;
		_size = size;
	}

	char *getData() { return _data; }
	int getSize() { return _size; }

private:
	char *_data;
	int  _size;
};


#endif //H_FRAME_DATA
