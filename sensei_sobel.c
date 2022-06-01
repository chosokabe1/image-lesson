#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 256
#define WIDTH  256

typedef unsigned char uchar;

void ipr_load_pgm(uchar image[][WIDTH], const char path[]);
void ipr_save_pgm(uchar image[][WIDTH], const char path[]);
void ipr_load_ppm(uchar image[][WIDTH][3], const char path[]);
void ipr_save_ppm(uchar image[][WIDTH][3], const char path[]);

void sobel_horizontal(uchar image[][WIDTH], double sobel[][WIDTH]);

int main(int argc, char *argv[])
{
    uchar src_image[HEIGHT][WIDTH];
    uchar dst_image[HEIGHT][WIDTH][3];
    double sobelX[HEIGHT][WIDTH];

    ipr_load_pgm(src_image, argv[1]);

    sobel_horizontal(src_image, sobelX);

    ipr_save_ppm(dst_image, "hoge.ppm");
    
    return 0;
}

void ipr_load_pgm(uchar image[][WIDTH], const char path[])
{
    char magic_number[2];
    int width, height;
    int max_intensity;
    FILE *fp;
    
    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "%s が開けませんでした．\n", path);
        exit(1);
    }
    
    fscanf(fp, "%c%c", &magic_number[0], &magic_number[1]);
    if (magic_number[0] != 'P' || magic_number[1] != '5') {
        fprintf(stderr, "%s はバイナリ型 PGM ではありません．\n", path);
        fclose(fp);
        exit(1);
    }
    
    fscanf(fp, "%d %d", &width, &height);
    if (width != WIDTH || height != HEIGHT) {
        fprintf(stderr, "画像のサイズが異なります．\n");
        fprintf(stderr, "  想定サイズ：WIDTH = %d, HEIGHT = %d\n", WIDTH, HEIGHT);
        fprintf(stderr, "  実サイズ：  width = %d, height = %d\n", width, height);
        fclose(fp);
        exit(1);
    }
    
    fscanf(fp, "%d", &max_intensity);
    if (max_intensity != 255) {
        fprintf(stderr, "最大階調値が不正な値です（%d）．\n", max_intensity);
        fclose(fp);
        exit(1);
    }
    
    fgetc(fp);  // 最大階調値の直後の改行コードを読み捨て

    fread(image, sizeof(uchar), HEIGHT * WIDTH, fp);
    
    fclose(fp);
}

void ipr_save_pgm(uchar image[][WIDTH], const char path[])
{
    FILE *fp;
    
    fp = fopen(path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "%s が開けませんでした．\n", path);
        exit(1);
    }
    
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", WIDTH, HEIGHT);
    fprintf(fp, "255\n");
    fwrite(image, sizeof(uchar), HEIGHT * WIDTH, fp);
    
    fclose(fp);
}

void ipr_load_ppm(uchar image[][WIDTH][3], const char path[])
{
    char magic_number[2];
    int width, height;
    int max_intensity;
    FILE *fp;
    
    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "%s が開けませんでした．\n", path);
        exit(1);
    }
    
    fscanf(fp, "%c%c", &magic_number[0], &magic_number[1]);
    if (magic_number[0] != 'P' || magic_number[1] != '6') {
        fprintf(stderr, "%s はバイナリ型 PPM ではありません．\n", path);
        fclose(fp);
        exit(1);
    }
    
    fscanf(fp, "%d %d", &width, &height);
    if (width != WIDTH || height != HEIGHT) {
        fprintf(stderr, "画像のサイズが異なります．\n");
        fprintf(stderr, "  想定サイズ：WIDTH = %d, HEIGHT = %d\n", WIDTH, HEIGHT);
        fprintf(stderr, "  実サイズ：  width = %d, height = %d\n", width, height);
        fclose(fp);
        exit(1);
    }
    
    fscanf(fp, "%d", &max_intensity);
    if (max_intensity != 255) {
        fprintf(stderr, "最大階調値が不正な値です（%d）．\n", max_intensity);
        fclose(fp);
        exit(1);
    }
    
    fgetc(fp);  // 最大階調値の直後の改行コードを読み捨て

    fread(image, sizeof(uchar), HEIGHT * WIDTH * 3, fp);
    
    fclose(fp);
}

void ipr_save_ppm(uchar image[][WIDTH][3], const char path[])
{
    FILE *fp;
    
    fp = fopen(path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "%s が開けませんでした．\n", path);
        exit(1);
    }
    
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", WIDTH, HEIGHT);
    fprintf(fp, "255\n");
    fwrite(image, sizeof(uchar), HEIGHT * WIDTH * 3, fp);
    
    fclose(fp);
}


void sobel_horizontal(uchar image[][WIDTH], double sobel[][WIDTH])
{
    /////
    // 四隅の微分値の計算
    /////
    
    // (0, 0) の計算
    sobel[0][0]
        = image[0][1] + 2 * image[0][1] + image[1][1]
        - image[0][0] - 2 * image[0][0] - image[1][0];
    
    // (0, W - 1) の計算
    sobel[0][WIDTH - 1]
        = image[0][WIDTH - 1] + 2 * image[0][WIDTH - 1] + image[1][WIDTH - 1]
        - image[0][WIDTH - 2] - 2 * image[0][WIDTH - 2] - image[1][WIDTH - 2];
    
    // (H - 1, 0) の計算
    sobel[HEIGHT - 1][0]
        = image[HEIGHT - 2][1] + 2 * image[HEIGHT - 1][1] + image[HEIGHT - 1][1]
        - image[HEIGHT - 2][0] - 2 * image[HEIGHT - 1][0] - image[HEIGHT - 1][0];
    
    // (H - 1, W - 1) の計算
    sobel[HEIGHT - 1][WIDTH - 1]
        = image[HEIGHT - 2][WIDTH - 1] + 2 * image[HEIGHT - 1][WIDTH - 1]
        + image[HEIGHT - 1][WIDTH - 1]
        - image[HEIGHT - 2][WIDTH - 2] - 2 * image[HEIGHT - 1][WIDTH - 2]
        - image[HEIGHT - 1][WIDTH - 2];
    
    /////
    // 四隅を除く端の微分値の計算
    /////
    
    // 上端である (0    , 1) -- (0    , W - 2) と
    // 下端である (H - 1, 1) -- (H - 1, W - 2) の計算
    for (int x = 1; x < WIDTH - 2; x++) {
        sobel[0][x]
            = image[0][x + 1] + 2 * image[0][x + 1] + image[1][x + 1]
            - image[0][x - 1] - 2 * image[0][x - 1] - image[1][x - 1];
        
        sobel[HEIGHT - 1][x]
            = image[HEIGHT - 2][x + 1] + 2 * image[HEIGHT - 1][x + 1] + image[HEIGHT - 1][x + 1]
            - image[HEIGHT - 2][x - 1] - 2 * image[HEIGHT - 1][x - 1] - image[HEIGHT - 1][x - 1];
    }

    // 左端である (1, 0)     -- (H - 2, 0)     と
    // 右端である (1, W - 1) -- (H - 2, W - 1) の計算
    for (int y = 1; y < HEIGHT - 2; y++) {
        sobel[y][0]
            = image[y - 1][1] + 2 * image[y][1] + image[y + 1][1]
            - image[y - 1][0] - 2 * image[y][0] - image[y + 1][0];
        
        sobel[y][WIDTH - 1]
            = image[y - 1][WIDTH - 1] + 2 * image[y][WIDTH - 1] + image[y + 1][WIDTH - 1]
            - image[y - 1][WIDTH - 2] - 2 * image[y][WIDTH - 2] - image[y + 1][WIDTH - 2];
    }
    
    /////
    // 端を除く領域の計算
    /////
    
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            sobel[y][x]
                = image[y - 1][x + 1] + 2 * image[y][x + 1] + image[y + 1][x + 1]
                - image[y - 1][x - 1] - 2 * image[y][x - 1] - image[y + 1][x - 1];
        }
    }

    /////
    // 正規化のために 0.125 倍する
    /////
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            sobel[y][x] *= 0.125;
        }
    }
}