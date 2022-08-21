#ifndef __COMMON_MATH_H__
#define __COMMON_MATH_H__

/**
 * �Ƚ���
 * ����������ֵ���ʽ�� x y
 * ��� x>y :  res = 1
 * ��� x<y :  res = -1
 * ��� x=y :  res = 0
 * ���������磬���� qsort �Ƚ����ص��к����á�
 * ���⣬�������ܹ������Ż�Ϊ�޷�֧���룬����û�з�����������ķ��ա�
 * ������һ������������������������������и����á�
 */
#define DREAM_DIFF_SIGN(x,y) (((x)>(y)) - ((x)<(y)))

/**
 * ������ֵȡ�ϴ�
 */
#define DREAM_MAX(a,b) ((a) > (b) ? (a) : (b))

/**
* ������ֵȡ�ϴ�
*/
#define DREAM_MAX3(a,b,c) FFMAX(FFMAX(a,b),c)

/**
* ������ֵȡ��С
*/
#define DREAM_MIN(a,b) ((a) > (b) ? (b) : (a))

/**
* ������ֵȡ��С
*/
#define DREAM_MIN3(a,b,c) DREAM_MIN(DREAM_MIN(a,b),c)

/**
* �������ͽ���������ֵ�Ľ���
*/
#define DREAM_SWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)

/**
* ��ȡһ������ĳ���
*/
#define DREAM_ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

/**
 * ���ģʽ�µ� 
 */
#define MKTAG(a,b,c,d)   ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

/**
* С��ģʽ�µ�
*/
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))


#endif //!__COMMON_MATH_H__