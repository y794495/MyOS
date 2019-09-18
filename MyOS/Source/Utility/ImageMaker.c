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

	//MyOS.img ���� ����
	if ((iTargetFd = open("MyOS.img", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE)) == -1)
	{
		fprintf(stderr, "[Error] MyOS.img open fail.\n");
		exit(-1);
	}
	//��Ʈ�δ��� ��� ��� ������ ��ũ �̹��� ���Ϸ� ����
	printf("[INFO] Copy boot loader to image file\n");
	if ((iSourceFd = open(argv[1], O_RDONLY | O_BINARY)) == -1)
	{
		fprintf(stderr, "[Error] %s open fail\n", argv[1]);
		exit(-1);
	}

	iSourceSize = CopyFile(iSourceFd, iTargetFd);
	close(iSourceFd);

	//���� ũ�⸦ ���� ũ�� 512����Ʈ�� ���߱� ���� �������� 0���� ü��
	iBootLoaderSize = AdjustInSectorSize(iTargetFd, iSourceSize);
	printf("[Info] %s size = [%d] and sector count = [%d]\n", argv[1], iSourceSize, iBootLoaderSize);

	
	//32��Ʈ Ŀ�� ������ ��� ��� ������ ��ũ �̹��� ���Ϸ� ����
	printf("[Info] Copy protected mode kernel to image file\n");
	if ((iSourceFd = open(argv[2], O_RDONLY | O_BINARY)) == -1)
	{
		fprintf(stderr, "[Error] %s open fail\n", argv[2]);
		exit(-1);
	}

	iSourceSize = CopyFile(iSourceFd, iTargetFd);
	close(iSourceFd);
	
	//���� ũ�⸦ ���� ũ�� 512����Ʈ�� ���߱� ���� �������� 0���� ü��
	iKernel32SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
	printf("[Info] %s size = [%d] and sector count = [%d]\n", argv[2], iSourceSize, iKernel32SectorCount);

	//64��Ʈ Ŀ�������� ��� ��� ������ ��ũ �̹��� ���Ϸ� ����
	printf("[Info] Copy IA-32e mode Kernel to image file\n");
	if ((iSourceFd = open(argv[3], O_RDONLY | O_BINARY)) == -1)
	{
		fprintf(stderr, "[Error] %s open fail\n", argv[3]);
		exit(-1);
	}

	iSourceSize = CopyFile(iSourceFd, iTargetFd);
	close(iSourceFd);

	//���� ũ�⸦ ���� ũ�� 512����Ʈ�� ���߱� ���� �������� 0���� ü��
	iKernel64SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
	printf("[Info] %s size = [%d] and sector count = [%d]\n", argv[3], iSourceSize, iKernel64SectorCount);

	//��ũ �̹����� Ŀ�� ���� ����
	printf("[Info] Start to write Kernel Information\n");
	//��Ʈ������ 5��° ����Ʈ���� Ŀ�ο� ���� ������ ����
	WriteKernelInformation(iTargetFd, iKernel32SectorCount + iKernel64SectorCount, iKernel32SectorCount);
	printf("[Info] Image file create complete\n");

	close(iTargetFd);
	return 0;
}

//���� ��ġ���� 512����Ʈ ��� ��ġ���� ���缭 0���� ü��
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

//��Ʈ�δ��� Ŀ�� ���� ����
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

// �ҽ����� ������ ��ǥ���Ͽ� �����ϰ� ũ�⸦ ������
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