#include <cstdio>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <chrono>

#define CUDA_RANGE 1000
#define BLOCK_SIZE 512
#define WARP_SIZE 32

struct point {
    float x;
    float y;
};

const int THREADS = 1 << 20;
const int THREADS_PER_BLOCK = 512;


__device__ float func(float x) {
    return cosf(3.0f * powf(x, 4.0f)) * sinf(5.0f * powf(x, 2.0f)) * powf(sinf(5.0f * x), 2.0f);

}

__device__ point max(point a, point b) {
    if (a.y > b.y) {
        return a;
    }
    return b;
}

__global__ void kernel(point *outData) {

    __shared__ point data[BLOCK_SIZE];
    int thread_id = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    float part = (float) CUDA_RANGE / (float) (blockDim.x * gridDim.x);
    float end = part * (float) (idx + 1);
    float x = part * (float) idx;

    point result{
            x,
            func(x)
    };

    while (x <= end) {
        float y = func(x);
        if (y > result.y) {
            result.y = y;
            result.x = x;
        }
        x += 1E-4f;
    }

    data[thread_id] = result;
    __syncthreads();

    for (int s = blockDim.x / 2; s > WARP_SIZE; s >>= 1) {
        if (thread_id < s)
            data[thread_id] = max(data[thread_id], data[thread_id + s]);
        __syncthreads();
    }

    if (thread_id < WARP_SIZE) {
        data[thread_id] = max(data[thread_id], data[thread_id + 32]);
        data[thread_id] = max(data[thread_id], data[thread_id + 16]);
        data[thread_id] = max(data[thread_id], data[thread_id + 8]);
        data[thread_id] = max(data[thread_id], data[thread_id + 4]);
        data[thread_id] = max(data[thread_id], data[thread_id + 2]);
        data[thread_id] = max(data[thread_id], data[thread_id + 1]);
    }

    if (thread_id == 0)
        outData[blockIdx.x] = data[0];
}

int main() {
    auto start = std::chrono::steady_clock::now();
    thrust::host_vector<point> H(THREADS / THREADS_PER_BLOCK);
    thrust::device_vector<point> D(THREADS / THREADS_PER_BLOCK);
    point *ptr = thrust::raw_pointer_cast(&D[0]);
    kernel<<<THREADS / THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(ptr);
    thrust::copy(D.begin(), D.end(), H.begin());

    point res{
            -1,
            -std::numeric_limits<float>::infinity()
    };

    for (int i = 0; i < H.size(); i++) {
        if (H[i].y > res.y) {
            res = H[i];
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "X: " << res.x << " Y: " << res.y << std::endl;
    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"
              << std::endl;

    return EXIT_SUCCESS;
}
