#include <cmath>
#include <vector>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

Model *model = NULL;
const int width = 1600;
const int height = 1600;

// 画一条 (x0, y0) 到 (x1, y1) 的 color 颜色的线段
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
  bool steep = false;
  // 如果线段比较陡峭（斜率大），则交换横纵坐标
  // 想象一下，假设一条线段的斜率非常大，举个例子，x0=2，x1=4；但是y0=3，y1=300。则使用下面循环插值时，实际上就画了三个点，中间会有很大的缝隙
  if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }
  // 确保(x0, y0)是处于左边的点
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  // 起点与终点的横纵坐标插值
  int dx = x1 - x0;
  int dy = y1 - y0;

  // 这并没有什么意义，意义在于下面的if中的比较
  int derror2 = std::abs(dy) * 2;
  int error2 = 0;
  // 先将要画的点赋值为起点
  int y = y0;
  for (int x = x0; x <= x1; x++) {
    // 如果比较陡峭，证明交换过，这次再交换回来
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error2 += derror2;
    // if (k*(dy/dx)>1/2)
    // 等价于下面的式子，也就是如果斜率累积大于0.5，就应该移动像素了
    if (error2 > dx) {
      // 在纵坐标（也可能是交换过后的横坐标）进行移动
      y += (y1 > y0 ? 1 : -1);
      // k*(dy/dx) -= 1
      error2 -= dx * 2;
    }
  }
}

int main() {
  model = new Model("obj/african_head.obj");
  TGAImage image(width, height, TGAImage::RGB);

  // 遍历面片
  for (int i = 0; i < model->nfaces(); i++) {
    // 获取面片
    std::vector<int> face = model->face(i);
    for (int j = 0; j < 3; j++) {
      // 获取面片三角形的两点，该两点之间有连边
      Vec3f v0 = model->vert(face[j]);
      Vec3f v1 = model->vert(face[(j + 1) % 3]);
      // 因为坐标范围为 [-1, 1]，因此 +1 以后 除以 2，转换为范围 [0, 1]，最后乘上 width/height
      int x0 = (v0.x + 1.0) * width / 2.0;
      int y0 = (v0.y + 1.0) * height / 2.0;
      int x1 = (v1.x + 1.0) * width / 2.0;
      int y1 = (v1.y + 1.0) * height / 2.0;
      line(x0, y0, x1, y1, image, white);
    }
  }

  image.flip_vertically();
  image.write_tga_file("output.tga");
  return 0;
}
