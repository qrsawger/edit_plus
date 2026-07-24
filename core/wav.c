#include "wav.h"

/* 加载并解析 WAV 文件 */
WAVData* wav_load(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    WAVHeader hdr;
    /* 读取固定头部（44 字节标准，但有些文件有额外字段） */
    fread(&hdr, sizeof(WAVHeader), 1, fp);

    /* 校验头部 */
    if (memcmp(hdr.chunkID, "RIFF", 4) != 0 ||
        memcmp(hdr.format, "WAVE", 4) != 0 ||
        memcmp(hdr.subchunk1ID, "fmt ", 4) != 0 ||
        hdr.audioFormat != 1) {  // 仅支持 PCM
        fclose(fp);
        return NULL;
    }

    /* 跳过可能的额外 fmt 字段 */
    if (hdr.subchunk1Size > 16) {
        fseek(fp, hdr.subchunk1Size - 16, SEEK_CUR);
    }

    /* 查找 data 块（有些 WAV 文件可能包含 LIST 等额外块） */
    uint32_t dataSize = 0;
    while (1) {
        char chunkID[4];
        uint32_t chunkSize;
        if (fread(chunkID, 4, 1, fp) != 1) break;
        if (fread(&chunkSize, 4, 1, fp) != 1) break;
        if (memcmp(chunkID, "data", 4) == 0) {
            dataSize = chunkSize;
            break;
        }
        fseek(fp, chunkSize, SEEK_CUR);
    }

    if (dataSize == 0) {
        fclose(fp);
        return NULL;
    }

    /* 读取 PCM 数据 */
    uint8_t *data = (uint8_t*)malloc(dataSize);
    if (!data) {
        fclose(fp);
        return NULL;
    }
    fread(data, 1, dataSize, fp);
    fclose(fp);

    /* 填充结果 */
    WAVData *wav = (WAVData*)malloc(sizeof(WAVData));
    wav->numChannels = hdr.numChannels;
    wav->sampleRate = hdr.sampleRate;
    wav->bitsPerSample = hdr.bitsPerSample;
    wav->data = data;
    wav->dataSize = dataSize;

    return wav;
}

/* 释放 WAV 数据 */
void wav_free(WAVData *wav) {
    if (wav) {
        if (wav->data) free(wav->data);
        free(wav);
    }
}