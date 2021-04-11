//
//	vVec.h			- Generic vector class.
//				Takehiko Terada (terada@sgi.co.jp)
//	1998/10/22 - ver 0.1	
//	1999/05/20 - ver 0.2
//

#include <stdio.h>
#include <iostream>
#include <math.h>

#define V_SQUARE(x)	((x)*(x))
#define V_SMALLVAL	(1.0e-15)
#define V_HUGEVAL	(3.40282347e+37)

class vVec2 {

  public:

    float vec[2];

    // Constructor
    vVec2() {
	set(0.0, 0.0);
    }

    vVec2(float _x, float _y) {
	set(_x, _y);
    };

    ~vVec2() {}

    float& operator [](int i) { return(vec[i]); }
    const float& operator [](int i) const { return(vec[i]); }

    int operator ==(const vVec2 &_v) const {
	return((vec[0] == _v[0]) &&
	       (vec[1] == _v[1]));
    }
    int operator !=(const vVec2 &_v) const { return !(*this == _v); }

    inline vVec2 operator -() const {
	return(vVec2(-vec[0], -vec[1]));
    }
    inline vVec2 operator +(const vVec2 &_v) const {
	return(vVec2(vec[0]+_v[0], vec[1]+_v[1]));
    }
    inline vVec2 operator -(const vVec2 &_v) const {
	return(vVec2(vec[0]-_v[0], vec[1]-_v[1]));
    }
    friend inline vVec2 operator *(float _s, const vVec2 &_v);
    friend inline vVec2 operator *(const vVec2 &_v, float _s);
    friend inline vVec2 operator *(const vVec2 &_v1, const vVec2 &_v2);
    friend inline vVec2 operator /(const vVec2 &_v, float _s);
    friend inline vVec2 operator /(const vVec2 &_v1, const vVec2 &_v2);

    vVec2& operator =(const vVec2 &_v) {
	vec[0] = _v[0];
	vec[1] = _v[1];
	return *this;
    }

    vVec2& operator *=(float _s) {
	vec[0] *= _s;
	vec[1] *= _s;
	return *this;
    }

    vVec2& operator /=(float _s) {
	_s = 1.0 / _s;
	return (*this *= _s);
    }

    vVec2& operator +=(const vVec2 &_v) {
	vec[0] += _v[0];
	vec[1] += _v[1];
	return *this;
    }

    vVec2& operator -=(const vVec2 &_v) {
	vec[0] -= _v[0];
	vec[1] -= _v[1];
	return *this;
    }

    void set(float _x, float _y) {
	vec[0] = _x;
	vec[1] = _y;
    }

    void copy(const vVec2 &_v) {
	*this = _v;
    }

    int equal(const vVec2 &_v) {
	return((vec[0] == _v[0]) &&
	       (vec[1] == _v[1]));
    };

    void negate(const vVec2 &_v) {
	vec[0] = -_v[0];
	vec[1] = -_v[1];
    }

    float dot(const vVec2 &_v) const {
	return((vec[0] * _v[0]) +
	       (vec[1] * _v[1]));
    }

    void add(const vVec2 &_v1, const vVec2 &_v2) {
	vec[0] = _v1[0] + _v2[0];
	vec[1] = _v1[1] + _v2[1];
    }

    void sub(const vVec2 &_v1, const vVec2 &_v2) {
	vec[0] = _v1[0] - _v2[0];
	vec[1] = _v1[1] - _v2[1];
    }

    float distance(const vVec2 &_v) const {
	return((float)sqrt(V_SQUARE(vec[0] - _v[0]) +
	       	       V_SQUARE(vec[1] - _v[1])));
    }

    void cross(const vVec2 &_v1, const vVec2 &_v2) {
	vec[0] = _v1[1] * _v2[2] - _v1[2] * _v2[1];
	vec[1] = _v1[2] * _v2[0] - _v1[0] * _v2[2];
    }

    float length(const vVec2 &_v) const {
	return((float)sqrt((vec[0] * _v[0]) +
			   (vec[1] * _v[1])));
    }

    float normalize() {
	float _s, _t;

	_s = length(*this);
	if (_s < V_SMALLVAL)
	    _t = V_HUGEVAL;
	else
	    _t = 1.0 / _s;
	
	*this *= _t;
	return(_s);
    }

    void print() {
	  printf("%f %f\n", vec[0], vec[1]);
    }

};

inline vVec2 operator *(float _s, const vVec2 &_v) {
    return(vVec2(_v[0]*_s, _v[1]*_s));
}

inline vVec2 operator *(const vVec2 &_v, float _s) {
    return(vVec2(_v[0]*_s, _v[1]*_s));
}

inline vVec2 operator *(const vVec2 &_v1, const vVec2 &_v2) {
    return(vVec2(_v1[0]*_v2[0], _v1[1]*_v2[0]));
}

inline vVec2 operator /(const vVec2 &_v, float _s) {
    _s = 1.0 / _s;
    return(vVec2(_v[0]*_s, _v[1]*_s));
}

inline vVec2 operator /(const vVec2 &_v1, const vVec2 &_v2) {
    return(vVec2(_v1[0]/_v2[0], _v1[1]/_v2[0]));
}

class vVec3 {

  public:

    float vec[3];

    // Constructor
    vVec3() {
	set(0.0, 0.0, 0.0);
    }

    vVec3(float _x, float _y, float _z) {
	set(_x, _y, _z);
    };

    ~vVec3() {}

    float& operator [](int i) { return(vec[i]); }
    const float& operator [](int i) const { return(vec[i]); }

    int operator ==(const vVec3 &_v) const {
	return((vec[0] == _v[0]) &&
	       (vec[1] == _v[1]) &&
	       (vec[2] == _v[2]));
    }
    int operator !=(const vVec3 &_v) const { return !(*this == _v); }

    inline vVec3 operator -() const {
	return(vVec3(-vec[0], -vec[1], -vec[2]));
    }
    inline vVec3 operator +(const vVec3 &_v) const {
	return(vVec3(vec[0]+_v[0], vec[1]+_v[1], vec[2]+_v[2]));
    }
    inline vVec3 operator -(const vVec3 &_v) const {
	return(vVec3(vec[0]-_v[0], vec[1]-_v[1], vec[2]-_v[2]));
    }
    friend inline vVec3 operator *(float _s, const vVec3 &_v);
    friend inline vVec3 operator *(const vVec3 &_v, float _s);
    friend inline vVec3 operator *(const vVec3 &_v1, const vVec3 &_v2);
    friend inline vVec3 operator /(const vVec3 &_v, float _s);
    friend inline vVec3 operator /(const vVec3 &_v1, const vVec3 &_v2);

    vVec3& operator =(const vVec3 &_v) {
	vec[0] = _v[0];
	vec[1] = _v[1];
	vec[2] = _v[2];
	return *this;
    }

    vVec3& operator *=(float _s) {
	vec[0] *= _s;
	vec[1] *= _s;
	vec[2] *= _s;
	return *this;
    }

    vVec3& operator /=(float _s) {
	_s = 1.0 / _s;
	return (*this *= _s);
    }

    vVec3& operator +=(const vVec3 &_v) {
	vec[0] += _v[0];
	vec[1] += _v[1];
	vec[2] += _v[2];
	return *this;
    }

    vVec3& operator -=(const vVec3 &_v) {
	vec[0] -= _v[0];
	vec[1] -= _v[1];
	vec[2] -= _v[2];
	return *this;
    }

    void set(float _x, float _y, float _z) {
	vec[0] = _x;
	vec[1] = _y;
	vec[2] = _z;
    }

    void copy(const vVec3 &_v) {
	*this = _v;
    }

    int equal(const vVec3 &_v) {
	return((vec[0] == _v[0]) &&
	       (vec[1] == _v[1]) &&
	       (vec[2] == _v[2]));
    };

    void negate(const vVec3 &_v) {
	vec[0] = -_v[0];
	vec[1] = -_v[1];
	vec[2] = -_v[2];
    }

    float dot(const vVec3 &_v) const {
	return((vec[0] * _v[0]) +
	       (vec[1] * _v[1]) +
	       (vec[2] * _v[2]));
    }

    void add(const vVec3 &_v1, const vVec3 &_v2) {
	vec[0] = _v1[0] + _v2[0];
	vec[1] = _v1[1] + _v2[1];
	vec[2] = _v1[2] + _v2[2];
    }

    void sub(const vVec3 &_v1, const vVec3 &_v2) {
	vec[0] = _v1[0] - _v2[0];
	vec[1] = _v1[1] - _v2[1];
	vec[2] = _v1[2] - _v2[2];
    }

    float distance(const vVec3 &_v) const {
	return((float)sqrt(V_SQUARE(vec[0] - _v[0]) +
	       	       V_SQUARE(vec[1] - _v[1]) +
	       	       V_SQUARE(vec[2] - _v[2])));
    }

    void cross(const vVec3 &_v1, const vVec3 &_v2) {
	vec[0] = _v1[1] * _v2[2] - _v1[2] * _v2[1];
	vec[1] = _v1[2] * _v2[0] - _v1[0] * _v2[2];
	vec[2] = _v1[0] * _v2[1] - _v1[1] * _v2[0];
    }

    float length(const vVec3 &_v) const {
	return((float)sqrt((vec[0] * _v[0]) +
			   (vec[1] * _v[1]) +
			   (vec[2] * _v[2])));
    }

    float normalize() {
	float _s, _t;

	_s = length(*this);
	if (_s < V_SMALLVAL)
	    _t = V_HUGEVAL;
	else
	    _t = 1.0 / _s;
	
	*this *= _t;
	return(_s);
    }

    void print() {
	  printf("%f %f %f\n", vec[0], vec[1], vec[2]);
    }

};

inline vVec3 operator *(float _s, const vVec3 &_v) {
    return(vVec3(_v[0]*_s, _v[1]*_s, _v[2]*_s));
}

inline vVec3 operator *(const vVec3 &_v, float _s) {
    return(vVec3(_v[0]*_s, _v[1]*_s, _v[2]*_s));
}

inline vVec3 operator *(const vVec3 &_v1, const vVec3 &_v2) {
    return(vVec3(_v1[0]*_v2[0], _v1[1]*_v2[0], _v1[2]*_v2[0]));
}

inline vVec3 operator /(const vVec3 &_v, float _s) {
    _s = 1.0 / _s;
    return(vVec3(_v[0]*_s, _v[1]*_s, _v[2]*_s));
}

inline vVec3 operator /(const vVec3 &_v1, const vVec3 &_v2) {
    return(vVec3(_v1[0]/_v2[0], _v1[1]/_v2[0], _v1[2]/_v2[0]));
}
