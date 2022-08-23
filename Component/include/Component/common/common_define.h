#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__

#include "Component/common/common_math.h"
#include <cstdint>

#define DREAM_ERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))

#define DREAM_ERROR_BSF_NOT_FOUND      DREAM_ERRTAG(0xF8,'B','S','F') ///< û���ҵ�������������
#define DREAM_ERROR_BUG                DREAM_ERRTAG( 'B','U','G','!') ///< �ڲ�bug, also see DREAM_ERROR_BUG2
#define DREAM_ERROR_BUFFER_TOO_SMALL   DREAM_ERRTAG( 'B','U','F','S') ///< buffer̫С��
#define DREAM_ERROR_DECODER_NOT_FOUND  DREAM_ERRTAG(0xF8,'D','E','C') ///< û���ҵ������� decoder
#define DREAM_ERROR_DEMUXER_NOT_FOUND  DREAM_ERRTAG(0xF8,'D','E','M') ///< û���ҵ������ demuxer
#define DREAM_ERROR_ENCODER_NOT_FOUND  DREAM_ERRTAG(0xF8,'E','N','C') ///< û���ҵ������� encoder
#define DREAM_ERROR_EOF                DREAM_ERRTAG( 'E','O','F',' ') ///< �ļ�����
#define DREAM_ERROR_EXIT               DREAM_ERRTAG( 'E','X','I','T') ///< Ҫ�������˳��������õĺ�����Ӧ����������
#define DREAM_ERROR_EXTERNAL           DREAM_ERRTAG( 'E','X','T',' ') ///< �ⲿ���һ�����
#define DREAM_ERROR_FILTER_NOT_FOUND   DREAM_ERRTAG(0xF8,'F','I','L') ///< û���ҵ������� filter
#define DREAM_ERROR_INVALIDDATA        DREAM_ERRTAG( 'I','N','D','A') ///< ��������ʱ������Ч����
#define DREAM_ERROR_MUXER_NOT_FOUND    DREAM_ERRTAG(0xF8,'M','U','X') ///< û���ҵ���װ�� muxer
#define DREAM_ERROR_OPTION_NOT_FOUND   DREAM_ERRTAG(0xF8,'O','P','T') ///< û���ҵ�ѡ��
#define DREAM_ERROR_PATCHWELCOME       DREAM_ERRTAG( 'P','A','W','E') ///< û���ҵ�ʵ�֣���ӭʹ�ò���
#define DREAM_ERROR_PROTOCOL_NOT_FOUND DREAM_ERRTAG(0xF8,'P','R','O') ///< û���ҵ�Э��
#define DREAM_ERROR_STREAM_NOT_FOUND   DREAM_ERRTAG(0xF8,'S','T','R') ///< û���ҵ���

#define DREAM_ERROR_BUG2               DREAM_ERRTAG( 'B','U','G',' ')
#define DREAM_ERROR_UNKNOWN            DREAM_ERRTAG( 'U','N','K','N') ///< δ֪����ͨ�������ⲿ��
#define DREAM_ERROR_EXPERIMENTAL       (-0x2bb2afa8)                  ///< ����Ĺ��ܱ����Ϊʵ���Եġ� ����������ʹ������������ strict_std_compliance��
#define DREAM_ERROR_INPUT_CHANGED      (-0x636e6701)                  ///< �����ڵ���֮�䷢���˱仯����Ҫ�������á�
#define DREAM_ERROR_OUTPUT_CHANGED     (-0x636e6702)                  ///< ����ڵ���֮�䷢���˱仯����Ҫ�������á�

/* HTTP & RTSP ���� */
#define DREAM_ERROR_HTTP_BAD_REQUEST   DREAM_ERRTAG(0xF8,'4','0','0')
#define DREAM_ERROR_HTTP_UNAUTHORIZED  DREAM_ERRTAG(0xF8,'4','0','1')
#define DREAM_ERROR_HTTP_FORBIDDEN     DREAM_ERRTAG(0xF8,'4','0','3')
#define DREAM_ERROR_HTTP_NOT_FOUND     DREAM_ERRTAG(0xF8,'4','0','4')
#define DREAM_ERROR_HTTP_OTHER_4XX     DREAM_ERRTAG(0xF8,'4','X','X')
#define DREAM_ERROR_HTTP_SERVER_ERROR  DREAM_ERRTAG(0xF8,'5','X','X')

using VLCBaseType = int16_t;

struct VLCElem 
{
	VLCBaseType sym, len;
};

#ifdef ARCH_LE
	#define BITSTREAM_READER_LE
	#define BITSTREAM_WRITER_LE
#else
#endif

#endif //!__COMMON_DEFINE_H__