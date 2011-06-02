/* 
 * File:   cDistortion.h
 * Author: benben
 *
 * Created on June 2, 2011, 4:52 PM
 */

#ifndef CDISTORTION_H
#define	CDISTORTION_H


/**
 * Encapsulates functions and algorithms for
 * noise transformation and distortion.
 */
struct cDistortion {
    /// [-inf,+inf] => [0,+1]
    static inline float sig(float x) {
        return (1.0f / (1.0f + (float) exp(-x)));
    }

    /// [-inf,+inf] => [0,+1]
    static inline float sigdbl(float x) {
        return copysign( 1.0f - (float) exp(-x*x), x );
    }

    /// [0,+1] => [0,+1] in n steps
    static inline float posterize(float x, int n) {
        return (floor(n * x) / n);
    }

    /// [0,+inf] => [0,+1], limiting to 1, ~5 => 0.993
    static inline float exposure(float x) {
        return (1.0f - (float) exp(-x));
    }

    /// [-inf,value-e] => 0, [value,+inf] => 1
    static inline float threshold(float x, float value) {
        return ((x < value) ? 0.0f : 1.0f);
    }

    // [-inf,+inf] => [-1,+1] _/\_
    static inline float rampUpDn(float min, float max, float value) {
        float slope = 2.0f / (max - min);
        // Ramp up to 1.
        float y = (value - min) * slope;
        // Ramp down from 1.
        y = (y < 1.0f) ? (y) : (2.0f - y);
        // Limit Bottom -1
        y = (y >= -1.0f) ? (y) : (-1.0f);
        return y;
    }

    // [-inf,+inf]^2 => [-1,+1] _/\_
    static inline float rampPyramid(float minx, float miny, float maxx, float maxy, float valuex, float valuey) {
        float a = rampUpDn(minx, maxx, valuex);
        float b = rampUpDn(miny, maxy, valuey);
        return fmin(a, b);
    }

    /// [0,+1] => [~min(a,b,c,d), ~max(a,b,c,d)]
    static inline float spline4(float x, float p0, float p1, float p2, float p3) {
        // x [0,+1] => u [-1,+2]
        float u = (x * 3.0f - 1.0f);
        float b0 = (u + 0.0f) * (u - 1.0f) * (u - 2.0f) * -0.166666f;
        float b1 = (u + 1.0f) * (u - 1.0f) * (u - 2.0f) * +0.500000f;
        float b2 = (u + 1.0f) * (u + 0.0f) * (u - 2.0f) * -0.500000f;
        float b3 = (u + 1.0f) * (u + 0.0f) * (u - 1.0f) * +0.166666f;
        float y = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
        return y;
    }

    /// [0,1] => [0,1], f(x) = max( (1-abs(x-0.57))^11, min((2.05*x)^12, min(0.61*x+0.197, 0.51) ) )
    static inline float continentalRidge(float x) {
        float a = pow(x*2.05f, 12);//1.1f * x;
        //float b = 0.5f * x + 0.26f;
        float b = 0.61f * x + 0.197f;
        float c = 0.51f;
        float d = pow(1.0f-fabs(x-0.57), 11);
        float h = fmax(d, fmin(a, fmin(b, c)));
        return h;
    }

    static inline void mix4(float alpha, float* zero4fv, float beta, float* one4fv, float* out4fv) {
        out4fv[0] = zero4fv[0] * alpha + one4fv[0] * beta;
        out4fv[1] = zero4fv[1] * alpha + one4fv[1] * beta;
        out4fv[2] = zero4fv[2] * alpha + one4fv[2] * beta;
        out4fv[3] = zero4fv[3] * alpha + one4fv[3] * beta;
    }

    static inline void mix3(float alpha, float* zero3fv, float beta, float* one3fv, float* out3fv) {
        out3fv[0] = zero3fv[0] * alpha + one3fv[0] * beta;
        out3fv[1] = zero3fv[1] * alpha + one3fv[1] * beta;
        out3fv[2] = zero3fv[2] * alpha + one3fv[2] * beta;
    }

    static inline void fade4(float alpha, float* zero4fv, float* one4fv, float* out4fv) {
        mix4(1.0f - alpha, zero4fv, alpha, one4fv, out4fv);
    }

    static inline void fade3(float alpha, float* zero3fv, float* one3fv, float* out3fv) {
        mix3(1.0f - alpha, zero3fv, alpha, one3fv, out3fv);
    }

    static inline float nm_to_R(int nm) {
        if (nm <= 390) return 1.0f;
        if (nm >= 440 && nm <= 510) return 0.0f;
        if (nm >= 580) return 1.0f;
        if (nm < 440) return (float(nm - 390) / float(440 - 390));
        return (float(nm - 510) / float(580 - 510));
    }

    static inline float nm_to_G(int nm) {
        if (nm <= 440) return 0.0f;
        if (nm >= 640) return 1.0f;
        if (nm >= 490 && nm <= 580) return 1.0f;
        if (nm < 490) return (float(nm - 440) / float(490 - 440));
        return (float(nm - 580) / float(640 - 580));
    }

    static inline float nm_to_B(int nm) {
        if (nm < 490) return 1.0f;
        if (nm > 510) return 0.0f;
        return (float(nm - 490) / float(510 - 490));
    }

#if 0
    // Approximate 1/sqrt(x) - put it here for now.
    static inline float InvSqrt(float x) {
        float xhalf = 0.5f * x;
        int i = *(int*) &x;
        i = 0x5f3759df - (i >> 1);
        x = *(float*) &i;
        x = x * (1.5f - xhalf * x * x);
        return x;
    }
#endif
};

#endif	/* CDISTORTION_H */

