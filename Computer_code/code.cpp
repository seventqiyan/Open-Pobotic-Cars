#include<cv.h>
#include<cxcore.h>
#include<highgui.h>

#include<cstdio>
#include<iostream>
using namespace std;

int main() {
	//����IplImageָ��
	IplImage* pFrame = NULL;
	IplImage* pCutFrame = NULL;
	IplImage* pCutFrImg = NULL;
	IplImage* pCutBkImg = NULL;
	//����CvMatָ��
	CvMat* pCutFrameMat = NULL;
	CvMat* pCutFrMat = NULL;
	CvMat* pCutBkMat = NULL;
	//����CvCaptureָ��
	CvCapture* pCapture = NULL;
	//����CvMemStorage��CvSegָ��
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* lines = NULL;
	//��ǰ֡��
	int nFrmNum = 0;
	//�ü�����ո߶�
	int CutHeight = 320;
	//��������
	cvNamedWindow("video", 1);
	//cvNamedWindow("background", 1);
	cvNamedWindow("foreground", 1);
	//�������ڳ�ʼλ��
	cvMoveWindow("video", 300, 30);
	cvMoveWindow("background", 100, 100);
	cvMoveWindow("foreground", 300, 370);
	//���ܴ����˳�
	if (!(pCapture = cvCaptureFromFile("1.avi"))) {
		fprintf(stderr, "Can not open video file\n");
		return -2;
	}
	//ÿ�ζ�ȡһ�����Ƶ
	while (pFrame = cvQueryFrame(pCapture)) {
		//����ROI�ü�ͼ��
		cvSetImageROI(pFrame, cvRect(0, CutHeight, pFrame->width, pFrame->height - CutHeight));
		nFrmNum++;
		//��һ��Ҫ�����ڴ�p
		if (nFrmNum == 1) {
			pCutFrame = cvCreateImage(cvSize(pFrame->width, pFrame->height - CutHeight), pFrame->depth, pFrame->nChannels);
			cvCopy(pFrame, pCutFrame, 0);
			pCutBkImg = cvCreateImage(cvSize(pCutFrame->width, pCutFrame->height), IPL_DEPTH_8U, 1);
			pCutFrImg = cvCreateImage(cvSize(pCutFrame->width, pCutFrame->height), IPL_DEPTH_8U, 1);

			pCutBkMat = cvCreateMat(pCutFrame->height, pCutFrame->width, CV_32FC1);
			pCutFrMat = cvCreateMat(pCutFrame->height, pCutFrame->width, CV_32FC1);
			pCutFrameMat = cvCreateMat(pCutFrame->height, pCutFrame->width, CV_32FC1);
			//ת���ɵ�ͨ��ͼ���ٴ���
			cvCvtColor(pCutFrame, pCutBkImg, CV_BGR2GRAY);
			cvCvtColor(pCutFrame, pCutFrImg, CV_BGR2GRAY);
			//ת���ɾ���
			cvConvert(pCutFrImg, pCutFrameMat);
			cvConvert(pCutFrImg, pCutFrMat);
			cvConvert(pCutFrImg, pCutBkMat);
		}
		else {
			//��ü���ͼ
			cvCopy(pFrame, pCutFrame, 0);
			//ǰ��ͼת��Ϊ�Ҷ�ͼ
			cvCvtColor(pCutFrame, pCutFrImg, CV_BGR2GRAY);
			cvConvert(pCutFrImg, pCutFrameMat);
			//��˹�˲��ȣ���ƽ��ͼ��
			cvSmooth(pCutFrameMat, pCutFrameMat, CV_GAUSSIAN, 3, 0, 0.0);
			//��ǰ֡������ͼ���
			cvAbsDiff(pCutFrameMat, pCutBkMat, pCutFrMat);
			//��ֵ��ǰ��ͼ
			cvThreshold(pCutFrMat, pCutFrImg, 35, 255.0, CV_THRESH_BINARY);
			//������̬ѧ�˲���ȥ������
			cvErode(pCutFrImg, pCutFrImg, 0, 1);
			cvDilate(pCutFrImg, pCutFrImg, 0, 1);
			//���±���
			cvRunningAvg(pCutFrameMat, pCutBkMat, 0.003, 0);
			//pCutBkMat = cvCloneMat(pCutFrameMat);
			//������ת��Ϊͼ���ʽ��������ʾ
			//cvConvert(pCutBkMat, pCutBkImg);
			cvCvtColor(pCutFrame, pCutBkImg, CV_BGR2GRAY);
			//canny�仯
			cvCanny(pCutFrImg, pCutFrImg, 50, 100);
#pragma region Hough���
			lines = cvHoughLines2(pCutFrImg, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 130, 40, 15);
			printf("Lines number: %d\n", lines->total);
			//����ֱ��
			for (int i = 0; i<lines->total; i++) {
				CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
				cvLine(pCutFrame, line[0], line[1], CV_RGB(255, 255, 0), 3, CV_AA);
			}
#pragma endregion
			//��ʾͼ��
			cvShowImage("video", pCutFrame);
			cvShowImage("background", pCutBkImg);
			cvShowImage("foreground", pCutFrImg);
			//�����¼����ո���ͣ����������ѭ��
			int temp = cvWaitKey(2);
			if (temp == 32) {
				while (cvWaitKey() == -1);
			}
			else if (temp >= 0) {
				break;
			}
		}
		//�ָ�ROI���򣨶����ȥ����
		cvResetImageROI(pFrame);
	}
	//���ٴ���
	cvDestroyWindow("video");
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");
	//�ͷ�ͼ��;���
	cvReleaseImage(&pCutFrImg);
	cvReleaseImage(&pCutBkImg);
	cvReleaseImage(&pCutFrame);
	cvReleaseMat(&pCutFrameMat);
	cvReleaseMat(&pCutFrMat);
	cvReleaseMat(&pCutBkMat);
	cvReleaseCapture(&pCapture);

	return 0;
}