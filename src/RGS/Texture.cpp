#include "Texture.h"
#include <stb-master/stb_image.h>

namespace RGS {

	Texture::Texture(const std::string& path)
		:m_Path(path)
	{
		Init();
	}

	Texture::~Texture()
	{
		if (m_Data)
			delete[] m_Data;
		m_Data = nullptr;

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				delete[] Mipmap[i][j];
				Mipmap[i][j] = nullptr;
			}
		}
	}

	Vec4 Texture::Sample(Vec2 texCoords) const
	{
		//点采样
		float vx = Clamp(texCoords.X, 0.0f, 1.0f);
		float vy = Clamp(texCoords.Y, 0.0f, 1.0f);

		int x = vx * (m_Width - 1) + 0.5f;
		int y = vy * (m_Height - 1) + 0.5f;

		int index = y * m_Width + x;
		return m_Data[index];
	}
	
	//void saveAsBMP(const std::string& filename, Vec4* data, int width, int height) {
	//	// BMP 文件头
	//	unsigned char bmpFileHeader[14] = {
	//		'B', 'M',               // 文件类型
	//		0, 0, 0, 0,             // 文件大小（稍后填充）
	//		0, 0,                   // 保留字段
	//		0, 0,                   // 保留字段
	//		54, 0, 0, 0             // 像素数据的偏移量（54 字节）
	//	};
	//	// BMP 信息头
	//	unsigned char bmpInfoHeader[40] = {
	//		40, 0, 0, 0,            // 信息头大小
	//		0, 0, 0, 0,             // 图像宽度（稍后填充）
	//		0, 0, 0, 0,             // 图像高度（稍后填充）
	//		1, 0,                   // 平面数（始终为 1）
	//		24, 0,                  // 每像素位数（24 位 RGB）
	//		0, 0, 0, 0,             // 压缩方式（0 表示无压缩）
	//		0, 0, 0, 0,             // 图像大小（稍后填充）
	//		0, 0, 0, 0,             // 水平分辨率（像素/米）
	//		0, 0, 0, 0,             // 垂直分辨率（像素/米）
	//		0, 0, 0, 0,             // 调色板颜色数（0 表示默认）
	//		0, 0, 0, 0              // 重要颜色数（0 表示所有颜色都重要）
	//	};
	//	// 设置图像宽度和高度
	//	int fileSize = 54 + width * height * 3; // 文件总大小
	//	std::memcpy(&bmpFileHeader[2], &fileSize, 4);
	//	std::memcpy(&bmpInfoHeader[4], &width, 4);
	//	std::memcpy(&bmpInfoHeader[8], &height, 4);
	//	// 创建文件
	//	std::ofstream file(filename, std::ios::out | std::ios::binary);
	//	// 写入文件头和信息头
	//	file.write(reinterpret_cast<const char*>(bmpFileHeader), sizeof(bmpFileHeader));
	//	file.write(reinterpret_cast<const char*>(bmpInfoHeader), sizeof(bmpInfoHeader));
	//	// 写入像素数据（BMP 的像素数据从左下角开始）
	//	for (int y = height - 1; y >= 0; --y) { // 从最后一行开始写
	//		for (int x = 0; x < width; ++x) {
	//			Vec4 color = data[y * width + x];
	//			unsigned char rgb[3];
	//			color.toRGB(rgb);
	//			file.write(reinterpret_cast<const char*>(rgb), 3);
	//		}
	//	}
	//	// 关闭文件
	//	file.close();
	//	//std::cout << "Saved BMP file: " << filename << std::endl;
	//}
	Vec4 Texture::MipmapSample(Vec2 texCoords, float Lx, float Ly)const
	{
		if (1) {
			Lx = Clamp(Lx, 0.0f, 3.0f);
			Ly = Clamp(Ly, 0.0f, 3.0f);
			if (Lx < 1 || Ly < 1) {
				float vx = Clamp(texCoords.X, 0.0f, 1.0f);
				float vy = Clamp(texCoords.Y, 0.0f, 1.0f);

				float x = vx * (m_Width - 1);
				float y = vy * (m_Height - 1);

				int x1 = ceil(x);
				int x0 = floor(x);
				int y1 = ceil(y);
				int y0 = floor(y);

				int index1 = y0 * m_Width + x0;
				int index2 = y0 * m_Width + x1;
				int index3 = y1 * m_Width + x0;
				int index4 = y1 * m_Width + x1;

				float xRatio = x1 - x;
				Vec4 u0 = Lerp(m_Data[index1], m_Data[index2], xRatio);
				Vec4 u1 = Lerp(m_Data[index3], m_Data[index4], xRatio);
				float yRatio = y1 - y;
				return Lerp(u0, u1, yRatio);
			}
			else {
				int firstXLevel = ceil(Lx);
				int secXLevel = floor(Lx);
				int firstYLevel = ceil(Ly);
				int secYLevel = floor(Ly);

				int scaleX = pow(2, firstXLevel - 1);
				int scaleY = pow(2, firstYLevel - 1);

				float vx = Clamp(texCoords.X, 0.0f, 1.0f);
				float vy = Clamp(texCoords.Y, 0.0f, 1.0f);

				int x1 = vx * (m_Width / scaleX - 1) + 0.5f;
				int y1 = vy * (m_Height / scaleY - 1) + 0.5f;
				int x2 = vx * (m_Width / (2 * scaleX) - 1) + 0.5f;
				int y2 = vy * (m_Height / scaleY - 1) + 0.5f;
				int x3 = vx * (m_Width / scaleX - 1) + 0.5f;
				int y3 = vy * (m_Height / (2 * scaleY) - 1) + 0.5f;
				int x4 = vx * (m_Width / (2 * scaleX) - 1) + 0.5f;
				int y4 = vy * (m_Height / (2 * scaleY) - 1) + 0.5f;

				int index1 = y1 * m_Width / scaleX + x1;
				int index2 = y2 * m_Width / (2 * scaleX) + x2;
				int index3 = y3 * m_Width / scaleX + x3;
				int index4 = y4 * m_Width / (2 * scaleX) + x4;

				Vec4 u0 = Mipmap[firstXLevel][firstYLevel][index1];
				Vec4 u1 = Mipmap[secXLevel][firstYLevel][index2];
				Vec4 u2 = Mipmap[firstXLevel][secYLevel][index3];
				Vec4 u3 = Mipmap[secXLevel][secYLevel][index4];

				float Xradio = secXLevel - Lx;
				float Yradio = secYLevel - Ly;

				Vec4 c1 = Lerp(u0, u1, Xradio);
				Vec4 c2 = Lerp(u2, u3, Xradio);
				return Lerp(c1, c2, Yradio);
			}
		}
		else {
			float vx = Clamp(texCoords.X, 0.0f, 1.0f);
			float vy = Clamp(texCoords.Y, 0.0f, 1.0f);

			int x = vx * (m_Width - 1) + 0.5f;
			int y = vy * (m_Height - 1) + 0.5f;
			int index = y * m_Width + x;
			return m_Data[index];
		}
	}

	void Texture::Init()
	{	
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
		ASSERT(data);

		m_Channels = channels;
		m_Height = height;
		m_Width = width;
		int size = height * width;
		m_Data = new Vec4[size];

		if (channels == 4) {
			for (int i = 0; i < size; ++i) {
				m_Data[i].X = UChar2Float(data[i * 4]);
				m_Data[i].Y = UChar2Float(data[i * 4 + 1]);
				m_Data[i].Z = UChar2Float(data[i * 4 + 2]);
				m_Data[i].W = UChar2Float(data[i * 4 + 3]);

			}
		}
		if (channels == 3) {
			for (int i = 0; i < size; ++i) {
				m_Data[i].X = UChar2Float(data[i * 3]);
				m_Data[i].Y = UChar2Float(data[i * 3 + 1]);
				m_Data[i].Z = UChar2Float(data[i * 3 + 2]);
				m_Data[i].W = 0.0f;

			}
		}
		if (channels == 2) {
			for (int i = 0; i < size; ++i) {
				m_Data[i].X = UChar2Float(data[i * 2]);
				m_Data[i].Y = UChar2Float(data[i * 2 + 1]);
				m_Data[i].Z = 0.0f;
				m_Data[i].W = 0.0f;

			}
		}
		Mipmap[0][0] = m_Data;
		if (channels == 1) {
			for (int i = 0; i < size; ++i) {
				m_Data[i].X = UChar2Float(data[i]);
				m_Data[i].Y = 0.0f;
				m_Data[i].Z = 0.0f;
				m_Data[i].W = 0.0f;

			}
		}
		if (IsMipMap) {
			//生成3x3mipmap
			for (int i = 1; i <= 3; ++i) {
				for (int j = 1; j <= 3; ++j) {
					if (i == 1 && j == 1) continue;
					
					int t_width = width / pow(2, i);
					int t_height = height / pow(2, j);
					Mipmap[i - 1][j - 1] = new Vec4[t_height* t_width + 1];
					for (int x = 0; x < t_width; x++) {
						for (int y = 0; y < t_height; y++) {
							Vec4 color = { 0,0,0,0 };
							for (int sampleX = 0; sampleX < i; ++sampleX) {
								for (int sampleY = 0; sampleY < j; ++sampleY) {
									color += m_Data[(y * j + sampleY) * width + (x * i + sampleX)];
								}
							}
							color = color/(i * j);
							Mipmap[i - 1][j - 1][y*t_width + x] = color;
						}
					}
					//if (Mipmap[i][j]) {
					//	std::ostringstream filename;
					//	filename << "Mipmap_Level_" << i + 1 << "x" << j + 1 << ".bmp";
					//	saveAsBMP(filename.str(), Mipmap[i][j], t_width, t_height);
					//}
				}
			}
		}
	}


}
