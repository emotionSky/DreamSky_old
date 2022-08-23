#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__

#include "Component/common/common_math.h"
#include <cstdint>

#define DREAM_ERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))

#define DREAM_ERROR_BSF_NOT_FOUND      DREAM_ERRTAG(0xF8,'B','S','F') ///< 没有找到比特流过滤器
#define DREAM_ERROR_BUG                DREAM_ERRTAG( 'B','U','G','!') ///< 内部bug, also see DREAM_ERROR_BUG2
#define DREAM_ERROR_BUFFER_TOO_SMALL   DREAM_ERRTAG( 'B','U','F','S') ///< buffer太小了
#define DREAM_ERROR_DECODER_NOT_FOUND  DREAM_ERRTAG(0xF8,'D','E','C') ///< 没有找到解码器 decoder
#define DREAM_ERROR_DEMUXER_NOT_FOUND  DREAM_ERRTAG(0xF8,'D','E','M') ///< 没有找到解封器 demuxer
#define DREAM_ERROR_ENCODER_NOT_FOUND  DREAM_ERRTAG(0xF8,'E','N','C') ///< 没有找到编码器 encoder
#define DREAM_ERROR_EOF                DREAM_ERRTAG( 'E','O','F',' ') ///< 文件结束
#define DREAM_ERROR_EXIT               DREAM_ERRTAG( 'E','X','I','T') ///< 要求立即退出，被调用的函数不应该重新启动
#define DREAM_ERROR_EXTERNAL           DREAM_ERRTAG( 'E','X','T',' ') ///< 外部库的一般错误
#define DREAM_ERROR_FILTER_NOT_FOUND   DREAM_ERRTAG(0xF8,'F','I','L') ///< 没有找到过滤器 filter
#define DREAM_ERROR_INVALIDDATA        DREAM_ERRTAG( 'I','N','D','A') ///< 处理输入时发现无效输入
#define DREAM_ERROR_MUXER_NOT_FOUND    DREAM_ERRTAG(0xF8,'M','U','X') ///< 没有找到封装器 muxer
#define DREAM_ERROR_OPTION_NOT_FOUND   DREAM_ERRTAG(0xF8,'O','P','T') ///< 没有找到选项
#define DREAM_ERROR_PATCHWELCOME       DREAM_ERRTAG( 'P','A','W','E') ///< 没有找到实现，欢迎使用补丁
#define DREAM_ERROR_PROTOCOL_NOT_FOUND DREAM_ERRTAG(0xF8,'P','R','O') ///< 没有找到协议
#define DREAM_ERROR_STREAM_NOT_FOUND   DREAM_ERRTAG(0xF8,'S','T','R') ///< 没有找到流

#define DREAM_ERROR_BUG2               DREAM_ERRTAG( 'B','U','G',' ')
#define DREAM_ERROR_UNKNOWN            DREAM_ERRTAG( 'U','N','K','N') ///< 未知错误，通常来自外部库
#define DREAM_ERROR_EXPERIMENTAL       (-0x2bb2afa8)                  ///< 请求的功能被标记为实验性的。 如果您真的想使用它，请设置 strict_std_compliance。
#define DREAM_ERROR_INPUT_CHANGED      (-0x636e6701)                  ///< 输入在调用之间发生了变化，需要重新配置。
#define DREAM_ERROR_OUTPUT_CHANGED     (-0x636e6702)                  ///< 输出在调用之间发生了变化，需要重新配置。

/* HTTP & RTSP 错误 */
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