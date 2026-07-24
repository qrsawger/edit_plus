#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* WAV 文件头结构（PCM 格式）—— 必须按 1 字节对齐，防止编译器填充 */
#pragma pack(push, 1)
typedef struct {
    /* RIFF 头 */
    char     chunkID[4];      // "RIFF"
    uint32_t chunkSize;
    char     format[4];       // "WAVE"

    /* fmt 子块 */
    char     subchunk1ID[4];  // "fmt "
    uint32_t subchunk1Size;
    uint16_t audioFormat;     // 1 = PCM
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    /* data 子块（注意：可能不在固定偏移，需查找） */
    /* 我们不在结构体中直接定义 data 子块，而是动态查找 */
} WAVHeader;
#pragma pack(pop)

/* 解码后的音频数据 */
typedef struct {
    uint16_t numChannels;
    uint32_t sampleRate;
    uint16_t bitsPerSample;
    uint8_t  *data;           // 裸 PCM 数据
    uint32_t dataSize;        // 字节数
} WAVData;

/* 加载并解析 WAV 文件（返回 WAVData*，调用者负责 wav_free） */
WAVData* wav_load(const char *filename);

/* 释放 WAV 数据 */
void wav_free(WAVData *wav);

#endif