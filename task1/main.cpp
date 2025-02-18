#include <fstream>
#include <vector>
#include <cstdint>
#include <math.h>
#include <iostream>

using namespace std;

const double pi2 = 4 * atan(1) * 2;

typedef struct WAV_HEADER {
    /* RIFF Chunk Descriptor */
    uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
    uint32_t ChunkSize;                     // RIFF Chunk Size
    uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
    uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
    uint16_t AudioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
                              // Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t NumOfChan = 1;   // Number of channels 1=Mono 2=Sterio
    uint32_t SamplesPerSec = 16000;   // Sampling Frequency in Hz
    uint32_t bytesPerSec = 16000 * 2; // bytes per second
    uint16_t blockAlign = 2;          // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample = 16;      // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
    uint32_t Subchunk2Size;                        // Sampled data length
  } wav_hdr;

void gen_data(std::vector<pair<int16_t, double>>& data, double freq, double freq_max,
              double sample_freq, uint64_t N, double volume_level,
              const std::vector<uint8_t>& info_bits, uint32_t bit_rate)
{
    data.resize(N);
    double phase = 0;
    double df = (freq_max - freq) / N * pi2 / sample_freq;
    uint32_t n_per_bit = N / bit_rate + (N % bit_rate)?1:0;
    double d_phase = freq * pi2 / sample_freq;
    int counter = 0;

    for(uint64_t i = 0; i < N; i++)
    {
        data[i].first = sin(phase + (info_bits[counter]) ? pi : 0) * (0xFFFF >> 1) * volume_level;
        data[i].second = i / sample_freq;
        d_phase += df;
        phase += d_phase;
        if (phase >= pi2)
            phase -= pi2;
        
    }

    return;
}

int main()
{
    std::ofstream file("data.txt");

    std::vector<pair<int16_t, double>> data;
    std::vector<uint8_t> info_bits = {1,1,0,1,1,0,1,1,0,1,1};
    double freq = 110;
    uint32_t bit_rate = 10;
    double freq_max = 220;
    double sample_freq = 100000;
    uint64_t N = 1000;
    double volume_level = 0.5;

    std::cout << "Type freq, freq_max, sample_freq, N, volume_level:" << std::endl;
    std::cin >> freq >> freq_max >> sample_freq >> N >> volume_level;

    gen_data(data, freq, freq_max, sample_freq, N, volume_level, info_bits, bit_rate);

    for (uint64_t i = 0; i < N; ++i)
        file << data[i].second << " " << data[i].first << "\n";


    wav_hdr wav;
    wav.SamplesPerSec = sample_freq;
    wav.bytesPerSec = 2 * sample_freq;
    wav.ChunkSize = N * 2 + sizeof(wav_hdr) - 8;
    wav.Subchunk2Size = N * 2 + sizeof(wav_hdr) - 44;

    std::ofstream out("test.wav", std::ios::binary);
    out.write(reinterpret_cast<const char *>(&wav), sizeof(wav));

    int16_t d;
    for (uint64_t i = 0; i < N; ++i) {
        d = data[i].first;
      // TODO: read/write in blocks
      out.write(reinterpret_cast<char *>(&d), sizeof(int16_t));
    }


    return 0;
}