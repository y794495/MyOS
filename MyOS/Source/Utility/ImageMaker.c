#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

int AdjustInSectorSize(int iFd, int iSourceSize);
void WriteKernelInformation(int iTargetFd, int iTotalKernelSectorCount, int iKernel32SectorCount);
int CopyFile(int iSourceFd, int iTargetFd);

int main(int argc, char* argv[])
{
	int iSourceFd;
	int iTargetFd;
	int iBootLoaderSize;
	int iKernel32SectorCount;
	int iKernel64SectorCount;
	int iSourceSize;

	if (argc < 4)
	{
		fprintf(stderr, "[Error] ImageMaker.exe BootLoader.bin Kernel32.bin Kernel64.bin\n");
		exit(-1);
	}

	//MyOS.img 파일 생성
	if ((iTargetFd = open("MyOS.img", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE)) == -1)
	{
		fprintf(stderr, "[Error] MyOS.img open fail.\n");
		exit(-1);
	}
	//부트로더를 열어서 모든 내용을 디스크 이미지 파일로 복사
	printf("[INFO] Copy boot loader to image file\n");
	if ((iSourceFd = open(argv[1], O_RDONLY | O_BINARY)) == -1)
	{
		fprintf(stderr, "[Error] %s open fail\n", argv[1]);
		exit(-1);
	}

	iSourceSize = CopyFile(iSourceFd, iTargetFd);
	close(iSourceFd);

	//파일 크기를 섹터 크기 512바이트로 맞추기 위해 나머지를 0으로 체움
	iBootLoaderSize = AdjustInSectorSize(iTargetFd, iSourceSize);
	printf("[Info] %s size = [%d] and sector count = [%d]\n", argv[1], iSourceSize, iBootLoaderSize);

	
	//32비트 커널 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
	printf("[Info] Copy protected mode kernel to image file\n");
	if ((iSourceFd = open(argv[2], O_RDONLY | O_BINARY)) == -1)
	{
		fprintf(stderr, "[Error] %s open fail\n", argv[2]);
		exit(-1);
	}

	iSourceSize = CopyFile(iSourceFd, iTargetFd);
	close(iSourceFd);
	
	//파일 크기를 섹터 크기 512바이트로 맞추기 위해 나머지를 0으로 체움
	iKernel32SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
	printf("[Info] %s size = [%d] and sector count = [%d]\n", argv[2], iSourceSize, iKernel32SectorCount);

	//64비트 커널파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
	printf("[Info] Copy IA-32e mode Kernel to image file\n");
	if ((iSourceFd = open(argv[3], O_RDONLY | O_BINARY)) == -1)
	{
		fprintf(stderr, "[Error] %s open fail\n", argv[3]);
		exit(-1);
	}

	iSourceSize = CopyFile(iSourceFd, iTargetFd);
	close(iSourceFd);

	//파일 크기를 섹터 크기 512바이트로 맞추기 위해 나머지를 0으로 체움
	iKernel64SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
	printf("[Info] %s size = [%d] and sector count = [%d]\n", argv[3], iSourceSize, iKernel64SectorCount);

	//디스크 이미지에 커널 정보 갱신
	printf("[Info] Start to write Kernel Information\n");
	//부트섹터의 5번째 바이트부터 커널에 대한 정보를 넣음
	WriteKernelInformation(iTargetFd, iKernel32SectorCount + iKernel64SectorCount, iKernel32SectorCount);
	printf("[Info] Image file create complete\n");

	close(iTargetFd);
	return 0;
}

//현재 위치부터 512바이트 배수 위치까지 맞춰서 0으로 체움
int AdjustInSectorSize(int iFd, int iSourceSize)
{
	int i;
	int iAdjustSizeToSector;
	char cCh;
	int iSectorCount;

	iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
	cCh = 0x00;

	if (iAdjustSizeToSector != 0)
	{
		iAdjustSizeToSector = 512 - iAdjustSizeToSector;
		printf("[Info] File size [%lu] and fill [%u] byte\n", iSourceSize, iAdjustSizeToSector);
		for (i = 0; i < iAdjustSizeToSector; i++)
		{
			write(iFd, &cCh, 1);
		}
	}
	else
	{
		printf("[Info] File size is aligned 512 byte\n");
	}

	iSectorCount = (iSourceSize + iAdjustSizeToSector) / BYTESOFSECTOR;
	return iSectorCount;
}

//부트로더에 커널 정보 삽입
void WriteKernelInformation(int iTargetFd, int iTotalSectorCount, int iKernel32SectorCount)
{
	unsigned short usData;
	long lPosition;

	lPosition = lseek(iTargetFd, 5, SEEK_SET);
	if (lPosition == -1)
	{
		fprintf(stderr, "lseek fail. Return value = %d, errno=%d, %d\n", lPosition, errno, SEEK_SET);
		exit(-1);
	}

	usData = (unsigned short)iTotalSectorCount;
	write(iTargetFd, &usData, 2);
	usData = (unsigned short)iKernel32SectorCount;
	write(iTargetFd, &usData, 2);

	printf("[Info] Total sector count except boot loader [%d]\n", iTotalSectorCount);
	printf("[Info] Total sector count of protected mode Kernel [%d]\n", iKernel32SectorCount);
}

// 소스파일 내용을 목표파일에 복사하고 크기를 돌려줌
int CopyFile(int iSourceFd, int iTargetFd)
{
	int iSourceFileSize;
	int iRead;
	int iWrite;
	char vcBuffer[BYTESOFSECTOR];

	iSourceFileSize = 0;
	while (1)
	{
		iRead = read(iSourceFd, vcBuffer, sizeof(vcBuffer));
		iWrite = write(iTargetFd, vcBuffer, iRead);

		if (iRead != iWrite)
		{
			fprintf(stderr, "[Error] iRead != iWrite...\n");
			exit(-1);
		}
		iSourceFileSize += iRead;

		if (iRead != sizeof(vcBuffer))
		{
			break;
		}
	}
	return iSourceFileSize;
}