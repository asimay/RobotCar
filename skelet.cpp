            // test5.cpp : 定义控制台应用程序的入口点。
//
 
 
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
#include <math.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;
//自定义的数据结构
typedef struct Inflexion   //拐点信息
{
 
    int width;
    int high;
    Inflexion * next;
    bool bflag;//标志是否是apar对的点
 
}Inflexion;
////////////////////////////////////////////////////////////////////////// 函数声明
//单像素轮廓化边界
void SinglePixelOutline(Mat &src,Mat&dst);
 
//求粗略的拐点  phead  指向拐点链表头部
void GetRoughInflexion(Mat &src,Mat&dst,Inflexion* &phead);
//插入拐点
void InsertList(int width ,int hight,Inflexion *&phead);
//释放链表
void DeleteList(Inflexion * &phead);
//去除多余的拐点，完成轮廓矢量化
void WipeInflexion(Inflexion *&phead);
//获得两个点的距离
int  GetPointGap(Inflexion *&pbegin,Inflexion *&p);
//获得三个点构成的三角形的其中一个夹角
double GetAngle(Inflexion*&pbegin,Inflexion*&pN0,Inflexion*&pNi);
//对原图画直线
void Drawline (Mat &src,Inflexion*&phead);
 
//对图像全部赋予一个值
void DrawPixel(Mat &src,int pixel);
 
 
/**
 *  main 函数
 */
 int  main( int argc, char** argv )
 {
 
     Mat src; Mat dst_single; Mat dst_Rough;
cv::VideoCapture cap(0); 
  //src=imread("lunkuo1.jpg",CV_LOAD_IMAGE_GRAYSCALE);

while(1) {cap >> src;
  //单像素轮廓化边界
   src.copyTo(dst_single);
  SinglePixelOutline(src,dst_single);
  
 Inflexion *phead=NULL;//指向拐点链表头部
  //求粗略的拐点
  dst_single.copyTo(dst_Rough);
 
  GetRoughInflexion(dst_single,dst_Rough,phead);
  //去除多余的拐点，完成轮廓矢量化
   WipeInflexion(phead);
   //画直线
  Mat dst_line;
  src.copyTo(dst_line);
  DrawPixel(dst_line,255);
  Drawline(dst_line,phead);
 
  //释放拐点链表
  DeleteList(phead);
 
  //结果显示
  imshow("src",src);
  imshow("result_single",dst_single);
  imshow("result_Rough",dst_Rough);
  imshow("result_line",dst_line);
     waitKey(10000);
}
     return 0;
  
}
 
 //单像素轮廓化边界
 //两个Mat必须规模一样
 void SinglePixelOutline(Mat &src,Mat&dst)
{
 
uchar *psrc=src.data;
uchar *pdst=dst.data;
int width=src.channels()* src.cols;
int height=src.rows;
//循环 非黑即白
for (int i=0;i<height;i++)
{
 
 
    for(int j=0;j<width;j++,psrc++,pdst++)
    {
 
        if (*psrc!=255)
        {
 
            *pdst=0;
         
}
     
}
 
}
 
 
 
//第一轮遍历 去除原图的噪点
Mat dst1;
dst.copyTo(dst1);
psrc=dst1.data;
pdst=dst.data;
for (int i=0;i<height;i++)
{
 
 
    for(int j=0;j<width;j++,psrc++,pdst++)
    {
 
        ///**psrc=0*/*pdst=255;
        if(i==0||j==0||i==height-1||j==width-1)  *psrc=255,*pdst=255;
 
        else
        {
 
 
            if(*psrc==0) //若为黑点
            {
 
     
                //求八领域中非黑点的个数
                int num=0;
 
                if (*(psrc-width-1)) num++;
                if (*(psrc-width)) num++;
                if (*(psrc-width+1)) num++;
 
                if (*(psrc-1)) num++;
                if (*(psrc+1)) num++;
 
                if (*(psrc+width-1)) num++;
                if (*(psrc+width)) num++;
                if (*(psrc+width+1)) num++;
                //领域中有非黑点则保留
                if (num)
                {
 
                    *pdst=255;
                 
}
                else *pdst=0;
 
             
             
}
 
         
}
 
     
}
 
 
}
 
 
 
//第二轮遍历 去除第一轮遍历图的左右或上下为白点的黑点
dst.copyTo(dst1);
psrc=dst1.data;
pdst=dst.data;
for (int i=0;i<height;i++)
{
 
 
    for(int j=0;j<width;j++,psrc++,pdst++)
    {
 
        ///**psrc=0*/*pdst=255;
        if(i==0||j==0||i==height-1||j==width-1)  *psrc=255,*pdst=255;
 
        else
        {
 
 
            if(*psrc==0) //若为黑点
            {
 
                if((*(psrc-1)&&*(psrc+1))||(*(psrc-width)&&*(psrc+width)))   *pdst=255;//若该点的左右或上下为白点则去除
 
                 
             
}
 
         
}
 
     
}
 
 
}
////第三轮遍历
dst.copyTo(dst1);
psrc=dst1.data;
pdst=dst.data;
for (int i=0;i<height;i++)
{
 
 
    for(int j=0;j<width;j++,psrc++,pdst++)
    {
 
        ///**psrc=0*/*pdst=255;
        if(i==0||j==0||i==height-1||j==width-1)  *psrc=255,*pdst=255;
         
        else
        if(*psrc==0) //若为黑点
        {
 
         {
     
            //求八领域中非黑点的个数
            int num=0;
             
            if (*(psrc-width-1)) num++;
            if (*(psrc-width)) num++;
            if (*(psrc-width+1)) num++;
             
            if (*(psrc-1)) num++;
            if (*(psrc+1)) num++;
 
            if (*(psrc+width-1)) num++;
            if (*(psrc+width)) num++;
            if (*(psrc+width+1)) num++;
             
           if (num<2||num>4)//若为黑点则 如果八邻域中<2 或者 >4个非黑点  则将黑点改为白点
           {
 
               *pdst=255;
            
}
           
}
          
}
         
}
         
     
}
 
 
}
 
 
 
 
 //插入拐点
void InsertList(int width ,int hight,Inflexion *&phead)
{
 
    Inflexion *p=new Inflexion;
    p->high= hight; //高度
    p->width=width;//宽度
    p->bflag=0;
    //插入链表
    if(phead) 
    {
 
        p->next=phead;
        phead=p;
     
}
    else 
    {
 
        phead=p;
        p->next=NULL;
     
}
 
}
 
 
 
 //求粗略的拐点  //拐点放在目标图中
     void GetRoughInflexion(Mat &src,Mat&dst,Inflexion* &phead)
 {
 
     uchar *psrc=src.data;
     uchar *pdst=dst.data;
     int width=src.channels()*src.cols;//width initialize
     int high= src.rows;
     int nCurDrt=0; //当前方向
     int nNextDrt=0;//下一个的方向
     int flag=100;//标志是否已经遍历
    // uchar* pCurPixel=0;//当前像素点
     //找到第一个和第二像素点并初始化方向和和标志  确定初始化的方向
    while (*psrc)
    {
 
        *psrc++,*pdst++;
     
}
    //标记当前的像素点为已经遍历
     *psrc =flag;
    //判断是八邻域的哪个方向
    if(!*(psrc+1))                {
 nCurDrt=1;psrc=psrc+1;pdst=pdst+1;
}
    else if(!*(psrc-width+1))     {
 nCurDrt=2;psrc=psrc-width+1;pdst=pdst-width+1;
}
     
    else if(!*(psrc-width))       {
 nCurDrt=3;psrc=psrc-width;pdst=pdst-width;
}
    else if(!*(psrc-width-1))     {
 nCurDrt=4;psrc=psrc-width-1;pdst=pdst-width-1;
}
     
    else if(!*(psrc-1))            {
 nCurDrt=5;psrc=psrc-1;pdst=pdst-1;
}
    else if(!*(psrc+width-1))      {
 nCurDrt=6;psrc=psrc+width-1;pdst=pdst+width-1;
}
     
    else if(!*(psrc+width))        {
 nCurDrt=7;psrc=psrc+width;pdst=pdst+width;
}
    else if(!*(psrc+width+1))      {
 nCurDrt=8;psrc=psrc+width+1;pdst=pdst+width+1;
}
    //标记当前的像素点为已经遍历
    *psrc =flag;
    //初始化邻域中已经标记的个数
    int nFlag=0;
    //while 遍历整张图并判断出粗略的拐点
   while(nFlag!=2)
   {
 
       //将当前点标记为已经遍历
       *psrc=flag;
       //遍历八领域，先把多出来的点去除
       if (*(psrc-width-1)==flag) {
*(psrc-width)=255;*(psrc-1)=255;
}
       if (*(psrc-width)==flag) {
 *(psrc-width-1)=255;*(psrc-width+1)=255;
}
       if (*(psrc-width+1)==flag) {
 *(psrc-width)=255;*(psrc+1)=255;
}
 
       if (*(psrc-1)==flag){
 *(psrc-width-1)=255;*(psrc+width-1)=255;
}
       if (*(psrc+1)==flag) {
 *(psrc-width+1)=255;*(psrc+width+1)=255;
}
 
       if (*(psrc+width-1)==flag) {
 *(psrc-1)=255;*(psrc+width)=255;
}
       if (*(psrc+width)==flag) {
 *(psrc+width-1)=255;*(psrc+width+1)=255;
}
       if (*(psrc+width+1)==flag) {
 *(psrc+width)=255;*(psrc+1)=255;
}
 
 
 
 
 
 
 
      //遍历八领域 ，并判断是否为拐点 ，最终转到下个领域点  ,将拐点插入 拐点链表中
       if(!*(psrc+1))    //方向为1，且像素未标记 若为拐点则标记目标图          
       {
 
           nNextDrt=1;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
              int   Cur_high= (psrc-src.data) /width; //高度
              int  Cur_width=(psrc-src.data)%width;//宽度
              InsertList(Cur_width,Cur_high,phead);
            
}
           nCurDrt=nNextDrt;
           psrc=psrc+1;
           pdst=pdst+1;
        
} 
 
 
       else if(!*(psrc-width+1))  //方向为2，且像素未标记 若为拐点则标记目标图
       {
 
           nNextDrt=2;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
 
 
           nCurDrt=nNextDrt;
           psrc=psrc-width+1;
           pdst=pdst-width+1;
        
}
 
       else if(!*(psrc-width))   //方向为3，且像素未标记 若为拐点则标记目标图  
       {
 
           nNextDrt=3;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
 
           nCurDrt=nNextDrt;
           psrc=psrc-width;
           pdst=pdst-width;
        
}
 
       else if(!*(psrc-width-1))  //方向为4，且像素未标记 若为拐点则标记目标图 
       {
  
           nNextDrt=4;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
           nCurDrt=nNextDrt;
           psrc=psrc-width-1;
           pdst=pdst-width-1;
        
}
 
 
       else if(!*(psrc-1))  //方向为5，且像素未标记  若为拐点则标记目标图         
       {
 
           nNextDrt=5;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
 
           nCurDrt=nNextDrt;
           psrc=psrc-1;
           pdst=pdst-1;
        
}
 
       else if(!*(psrc+width-1)) //方向为6，且像素未标记 若为拐点则标记目标图    
       {
  
           nNextDrt=6;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
            
           nCurDrt=nNextDrt;
           psrc=psrc+width-1;
           pdst=pdst+width-1;
        
}
 
       else if(!*(psrc+width)) //方向为7，且像素未标记  若为拐点则标记目标图      
       {
 
           nNextDrt=7;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
 
           nCurDrt=nNextDrt;
           psrc=psrc+width;
           pdst=pdst+width;
        
}
 
 
       else if(!*(psrc+width+1))  //方向为8，且像素未标记  若为拐点则标记目标图   
       {
  
           nNextDrt=8;
           if(nCurDrt!=nNextDrt)
           {
 
               *pdst=50 ;
               //将拐点插入 拐点链表中
               int   Cur_high= (psrc-src.data) /width; //高度
               int  Cur_width=(psrc-src.data)%width;//宽度
               InsertList(Cur_width,Cur_high,phead);
            
}
 
           nCurDrt=nNextDrt;
           psrc=psrc+width+1;
           pdst=pdst+width+1;
        
}
 
       //计算当前像素点的 八邻域 中已经标记的个数 nFlag
        nFlag=0;
       if (*(psrc-width-1)==flag)  nFlag++;
       if (*(psrc-width)==flag) nFlag++;
       if (*(psrc-width+1)==flag) nFlag++;
 
       if (*(psrc-1)==flag) nFlag++;
       if (*(psrc+1)==flag) nFlag++;
 
       if (*(psrc+width-1)==flag) nFlag++;
       if (*(psrc+width)==flag) nFlag++;
       if (*(psrc+width+1)==flag) nFlag++;
 
         
    
}
 
   //将当前点标记为已经遍历
*psrc=flag;
//判断该点是否为拐点
//遍历八领域 ，并判断是否为拐点 ，若是则标记
if(*(psrc+1)==flag)    //方向为1， 若为拐点则标记目标图          
{
 
    nNextDrt=1;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
} 
 
 
else if(*(psrc-width+1)==flag)  //方向为2， 若为拐点则标记目标图
{
 
    nNextDrt=2;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
}
 
else if(*(psrc-width)==flag)   //方向为3， 若为拐点则标记目标图  
{
 
    nNextDrt=3;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
}
 
else if(*(psrc-width-1)==flag)  //方向为4， 若为拐点则标记目标图 
{
  
    nNextDrt=4;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
}
 
 
else if(*(psrc-1)==flag)  //方向为5， 若为拐点则标记目标图         
{
 
    nNextDrt=5;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
}
 
else if(*(psrc+width-1)==flag) //方向为6， 若为拐点则标记目标图    
{
  
    nNextDrt=6;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
}
else if(*(psrc+width)==flag) //方向为7，  若为拐点则标记目标图      
{
 
    nNextDrt=7;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
}
 
 
else if(*(psrc+width+1)==flag)  //方向为8，  若为拐点则标记目标图   
{
  
    nNextDrt=8;
    if(nCurDrt!=nNextDrt)
    {
 
        *pdst=50 ;
        //将拐点插入 拐点链表中
        int   Cur_high= (psrc-src.data) /width; //高度
        int  Cur_width=(psrc-src.data)%width;//宽度
        InsertList(Cur_width,Cur_high,phead);
     
}
 
  
}
 
 
 
}
 
//释放链表
void DeleteList(Inflexion * &phead)
{
 
    Inflexion *p=phead;
    Inflexion *pnext=NULL;
    while (p)
    {
 
    pnext=p->next;
    delete p;
    p=pnext;
     
}
 
 
}
 
 
 
//获得两个点的距离
int  GetPointGap(Inflexion *&pbegin,Inflexion *&p)
{
 
    // 当前拐点至始点的距离平方
    int nhighgap=abs(p->high-pbegin->high);
    int nwidthgap=abs(p->width-pbegin->width);
    int L= nhighgap*nhighgap+nwidthgap*nwidthgap;
    return L;
 
}
 
//第一个参数为起始点，第二 No，第三个Ni
//获得三个点构成的三角形的其中一个夹角的度数  通过tan 的值
 
double GetAngle(Inflexion*&pbegin,Inflexion*&pN0,Inflexion*&pNi)
{
 
    //int a=GetPointGap(pbegin,pN0); //pN0到pbegin的距离
    //int b=GetPointGap(pN0,pNi);//pNi到pN0的距离
    //int c=GetPointGap(pNi,pbegin);//pNi到pbegin的距离
    //int value1=(a+c-b)*(a+c-b);
    //int value2=4*a*c;
    //float value=value1/value2;
    //
    //return value;
    float nN0highgap=abs(pN0->high-pbegin->high);
    float nN0widthgap=abs(pN0->width-pbegin->width);
    float value1=nN0highgap/nN0widthgap;
    float radian_N0=atanf(value1);
 
    float nNihighgap =abs(pNi->high-pbegin->high);
    float nNiwidthgap=abs(pNi->width-pbegin->width);
    float value2=nNihighgap/nNiwidthgap;
    float radian_Ni=atanf(value2);
    float value=fabsf(radian_Ni-radian_N0);
    return value;
 
 
}
 
 
 
 //去除多余的拐点，完成轮廓矢量化
 void WipeInflexion(Inflexion *&phead)
 {
 
     //初始化遍历的起始点
     Inflexion *pbegin=phead;
     int flag=1; //标识是否遍历完
     //设置排除拐点的阈值
     int TL=15*15;
     double TQ=0.06;/*3969*/;//20*3.14/180= 0.3490658504    15*3.14/180=0.2617993878  10*3.14/180=0.1745329252    5度 的偏角
     //开始循环，遍历
     while(flag)
     {
 
         //从开始点的下一个点开始遍历
         Inflexion *pN0=pbegin->next;
         if(pN0)  //存在PN0
         {
 
 
         // 当前拐点至始点的距离平方
         int L=GetPointGap(pbegin,pN0);
 
         //找出N0,即第一个L>TL  L=拐点与始点的距离，TL=15  ,寻找 PN0
         while(L<TL&&pN0->next)
         {
 
         Inflexion *q=NULL;
         q=pN0;
         pN0=pN0->next;
         pbegin->next=pN0;
         L=GetPointGap(pbegin,pN0);
         delete q;
          
}
         //判断是否到了终点
         if (L>=TL&&pN0->next) //没到终点  将该点设为pNo 且 N0->next 不为空
         {
 
          //从N0的下一个点开始遍历
          Inflexion *pNi=pN0->next;
         //找出Ni  角度大于TQ的点
         double angle=GetAngle(pbegin,pN0,pNi);
          while(angle<TQ&&pNi->next)  //
          {
 
              if (pN0->next!=pNi)
              {
 
                  delete pN0->next;
                  pN0->next=pNi;
               
}
              pNi=pNi->next;
              angle =GetAngle(pbegin,pN0,pNi);
           
}
 
          if (angle>TQ)  //没到终点  更新pbegin 将前一拐点置为新的起点 删除pN0
          {
 
              pbegin->next=pN0->next;
              pbegin=pN0->next;
              delete pN0;
           
}
          else  //到了终点//删除pN0->next的点 即当前pNi所指的点 
          {
 
            flag=0;
            pN0->next=NULL;
            delete pNi;
           
}
 
 
 
          
}
 
         else//到了终点  //pbegin后的点删除
         if(!pN0->next)
         {
 
           flag=0;
           pbegin->next=NULL;
           delete pN0;
          
}
 
          
}
 
 
      
}
 
  
}
 
 //对原图画直线
 void Drawline (Mat &src,Inflexion*&phead)
 {
 
     Inflexion *p=phead;
     Point2i dot1,dot2;
   while(p->next)
   {
 
     dot1.x=p->width;
     dot1.y=p->high;
     dot2.x=p->next->width;
     dot2.y=p->next->high;
     line(src,dot1,dot2,0);
     //拐点变为255
    /* *(src.data+dot1.x+dot1.y*src.cols)=0;*/
     p=p->next;
    
}
   dot1.x=p->width;
   dot1.y=p->high;
   dot2.x=phead->width;
   dot2.y=phead->high;
   line(src,dot1,dot2,0);
   //拐点变为255
   /**(src.data+dot1.x+dot1.y*src.cols)=0;*/
  
}
 //对图像全部赋予一个值
 void DrawPixel(Mat &src,int pixel)
 {
 
   uchar *p=src.data;
   int width=src.channels()*src.cols;
   int heitht=src.rows;
   for (int i=0;i<heitht;i++)
 
   {
 
       for(int j=0;j<width;j++)
       {
 
           *p=pixel;
           p++;
        
}
 
    
}
  
}          
