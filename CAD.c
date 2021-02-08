#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "strlib.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>
#include <math.h>

/*���水ťλ��*/
#define chooseStartX 0.25
#define chooseStopX 0.8
#define chooseStartY 0.6
#define chooseStopY 0.9

#define rectangleHeight 0.3
#define rectangleWidth 0.55

#define lineStartX 0.9
#define lineStopX 1.45
#define lineStartY 0.6
#define lineStopY 0.9

#define rectangleStartX 1.6
#define rectangleStopX 2.15
#define rectangleStartY 0.6
#define rectangleStopY 0.9

#define ellipseStartX 2.3
#define ellipseStopX 2.85
#define ellipseStartY 0.6
#define ellipseStopY 0.9

#define textStartX 3
#define textStopX 3.55
#define textStartY 0.6
#define textStopY 0.9

#define deleteStartX 3.7
#define deleteStopX 4.25
#define deleteStartY 0.6
#define deleteStopY 0.9

#define deltax 0.05
#define deltay 0.05

#define TIMER_BLINK500  1     /*500ms��ʱ���¼���־��*/

enum{line,rectangle,ellipse,null};/*ͼ������*/ 

static int changenow=0;/*����ѡ�е�ͼ��*/
static int num=0;/*ͼ���ܸ���*/ 
static int textnum = -1;/*�ַ����ܸ���*/ 
static int textnow = 0;/*ѡ�е��ַ����������е�λ��*/

/*�ж��Ƿ�������״̬*/
static bool ischoose = FALSE;
static bool isChange=FALSE;
static bool isdelete=FALSE;
static bool startdelete=FALSE;
static bool startDeleteText=FALSE;

/*�ж��Ƿ�������״̬*/
static bool isDrawText = FALSE;
static bool isDrawLine = FALSE;
static bool isDrawEllipse = FALSE;
static bool isDrawRectangle = FALSE;

/*�ж��Ƿ�����ƶ�״̬*/
static bool isMoveEllipse = FALSE;
static bool isMoveText = FALSE;
static bool isMoveLine = FALSE;
static bool isMoveRectangle = FALSE;

/*�ж��Ƿ�ʼ����*/
static bool startDrawLine = FALSE;
static bool startDrawRectangle = FALSE;
static bool startDrawEllipse = FALSE;
static bool startDrawText = FALSE;
	
/*�ж��Ƿ����ı��С״̬*/
static bool changeLine = FALSE;
static bool changeEllipse = FALSE;
static bool changeRectangle = FALSE;
static bool changeText = FALSE;

/*�ж��Ƿ����ѡ��״̬*/
static bool chooseLine = FALSE;
static bool chooseRectangle = FALSE;
static bool chooseEllipse = FALSE;
static bool chooseText = FALSE;
	
static int timerID;

static double sx=0.0, sy=0.0, sx1=0.0, sy1=0.0;/*����ϴ�λ�á�δ����ǰλ��*/

static double gx,gy;/*���λ��*/

static bool isBlink = FALSE;/*�Ƿ���˸*/ 

typedef struct node save;
static int count=0;/*ͼ������*/

struct node
{
	double x1;
	double y1;
	double x2;
	double y2;
	int type;
};/*ͼ��λ��������*/

save onScreen[100];/*��Ļ�ϵ�ͼ��*/

struct text
{
	char texts[100];
	int textlen;
	int current;
	double textx, texty, textx1, texty1;
};/*�ַ�������*/

struct text alltext[100];/*��Ļ���ַ�������*/

void Layout();/*�������ú���*/

void DrawRectangle(double x,double y);/*������*/

bool inBox(double x0, double y0, double x1, double x2, double y1, double y2);/*�ж��Ƿ���ѡ����*/

void KeyboardEventProcess(int key,int event);/*������Ϣ�ص�����*/
void CharEventProcess(char c);/*�ַ���Ϣ�ص�����*/
void MouseEventProcess(int x, int y, int button, int event);/*�����Ϣ�ص�����*/
void TimerEventProcess(int timerID);/*��ʱ����Ϣ�ص�����*/

void Main()
{
	InitGraphics();        	
	
	
	registerMouseEvent(MouseEventProcess);
	registerKeyboardEvent(KeyboardEventProcess);
	registerCharEvent(CharEventProcess);
	registerTimerEvent(TimerEventProcess);

	SetPenColor("Black"); 
    SetPenSize(1);
    
    startTimer(TIMER_BLINK500, 500);/*500ms��ʱ������*/
 
    
    Layout();
}

/*���水ť����*/ 
void Layout()
{
	MovePen(chooseStartX,chooseStopY);
	DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(lineStartX,lineStopY);
	DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(rectangleStartX,rectangleStopY);
	DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(ellipseStartX,ellipseStopY);
	DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(textStartX,textStopY);
	DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(deleteStartX,deleteStopY);
	DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(0.3,0.7);
	DrawTextString("ѡ��");
	MovePen(1.0,0.7);
	DrawTextString("ֱ��");
	MovePen(1.7,0.7);
	DrawTextString("����");
	MovePen(2.4,0.7);
	DrawTextString("��Բ");
	MovePen(3.1,0.7);
	DrawTextString("����");
	MovePen(3.8,0.7); 
	DrawTextString("ɾ��"); 
}

/*�ж��Ƿ���ѡ����*/ 
bool inBox(double x0, double y0, double x1, double x2, double y1, double y2)
{
	return (x0 >= x1 && x0 <= x2 && y0 >= y1 && y0 <= y2)||
	       (x0 >= x1 && x0 <= x2 && y0 <= y1 && y0 >= y2)||
		   (x0 <= x1 && x0 >= x2 && y0 >= y1 && y0 <= y2)||
		   (x0 <= x1 && x0 >= x2 && y0 <= y1 && y0 >= y2);
}

/*������*/ 
void DrawRectangle(double x,double y)
{
	DrawLine(x,0);
	DrawLine(0,-y);
	DrawLine(-x,0);
	DrawLine(0,y);
}

/*ѡ��״̬�£��Ҽ��ı��С*/
void MouseEventProcess(int x, int y, int button, int event)
{
	double mx, my;
	static double lx, ly;

	static double rx, ry;
	static double r = 0.2;
    
    int j=0;
     
	mx = ScaleXInches(x);
	my = ScaleYInches(y);
	
	
	switch(event){
		case BUTTON_DOWN:
			switch(button){
				case LEFT_BUTTON:
				      SetEraseMode(TRUE);/*�������*/
	 	              MovePen(gx,gy);
	 	              DrawLine(0,-GetFontHeight());
	 	              SetEraseMode(FALSE);
				 
				     if(isDrawText){
			        	startDrawText = TRUE;
			        	sx=mx,sy=my;
			        	sx1=mx,sy1=my;
			            MovePen(sx1,sy1);
			            textnum++;
			            alltext[textnum].textx = sx1;
 	 	                            alltext[textnum].texty = sy1;
 	 	                            alltext[textnum].texty1 = sy1-GetFontHeight();
 	 	                            alltext[textnum].textx1 = sx1;
 	 	                            alltext[textnum].textlen = 0;
 	 	                            alltext[textnum].current = 0;
 	 	                            gx = alltext[textnum].textx;
 	 	                            gy = alltext[textnum].texty;
 	 	                            textnow = textnum;
			        }
				     if(inBox(mx,my,lineStartX,lineStartX+rectangleWidth,lineStopY,lineStopY-rectangleHeight)){
			        	isDrawLine = TRUE;           /*�жϽ��뻭ֱ��ģʽ*/
			        	isDrawRectangle = FALSE;
			        	isDrawEllipse = FALSE;
			        	ischoose = FALSE;
			        	isDrawText = FALSE;
			        	isdelete = FALSE;
			        	startDrawText = FALSE;
			        	startDeleteText = FALSE;
			        	isMoveText = FALSE;
			        }else if(inBox(mx,my,rectangleStartX,rectangleStartX+rectangleWidth,rectangleStopY,rectangleStopY-rectangleHeight)){
			        	isDrawLine = FALSE;
			        	isDrawRectangle = TRUE;     /*�жϽ��뻭����ģʽ*/
			        	isDrawEllipse = FALSE;
			        	ischoose = FALSE;
			        	isDrawText = FALSE;
			        	isdelete = FALSE;
			        	startDrawText = FALSE;
			        	startDeleteText = FALSE;
			        	isMoveText = FALSE;
			        }else if(inBox(mx,my,ellipseStartX,ellipseStartX+rectangleWidth,ellipseStopY,ellipseStopY-rectangleHeight)){
			        	isDrawLine = FALSE;
			        	isDrawRectangle = FALSE;    /*�жϽ��뻭��Բģʽ*/
			        	isDrawEllipse = TRUE;
						ischoose = FALSE;
						isDrawText = FALSE;
						isdelete = FALSE;
						startDrawText = FALSE;
						startDeleteText = FALSE;
						isMoveText = FALSE;
			        }else if(inBox(mx,my,chooseStartX,chooseStartX+rectangleWidth,chooseStopY,chooseStopY-rectangleHeight)){
			        	ischoose = TRUE;
			        	isDrawLine = FALSE;        /*�жϽ���ѡ��ģʽ*/
			        	isDrawRectangle = FALSE;
			        	isDrawEllipse = FALSE;
			        	isDrawText = FALSE;
			        	isdelete = FALSE;
			        	startDrawText = FALSE;
			        	startDeleteText = FALSE;
			        }else if(inBox(mx,my,textStartX,textStartX+rectangleWidth,textStopY,textStopY-rectangleHeight)){
			        	ischoose = FALSE;
			        	isDrawLine = FALSE;        /*�жϽ�������ģʽ*/
			        	isDrawRectangle = FALSE;
			        	isDrawEllipse = FALSE;
			        	isDrawText = TRUE;
			        	isdelete = FALSE;
			        	startDeleteText = FALSE;
			        	isMoveText = FALSE;
			        }else if(inBox(mx,my,deleteStartX,deleteStartX+rectangleWidth,deleteStopY,deleteStopY-rectangleHeight)){
			        	ischoose = FALSE;
			        	isDrawLine = FALSE;        /*�жϽ���ɾ��ģʽ*/
			        	isDrawRectangle = FALSE;
			        	isDrawEllipse = FALSE;
			        	isDrawText = FALSE;
			        	isdelete = TRUE;
			        	startDrawText = FALSE;
			        	isMoveText = FALSE;
			        }else {if(ischoose) {
		    		    int i;
		    		    double x1,x2,y1,y2;
		    		    for(i=0;i<num;i++){  /*�ж�ѡ���ͼ��*/
		    			    x1=onScreen[i].x1;
						    x2=onScreen[i].x2;
						    y1=onScreen[i].y1;
						    y2=onScreen[i].y2;
						    
						    if(onScreen[i].type==rectangle){
		    				    if(inBox(mx,my,x1,x2,y1,y2)){
						   		    isMoveRectangle=TRUE;
						   		    isMoveLine=FALSE;
						   		    isMoveEllipse=FALSE;
						   		    changenow=i;
						   		    lx=x1,ly=y1;
						   		    sx=mx,sy=my;
						   		    sx1=mx,sy1=my;
						   		    rx=x2-x1,ry=y1-y2;
						   		    break;
							    }
						    }
					 		else if(onScreen[i].type==ellipse){
		    				    if(inBox(mx,my,x1,x2,y1,y2)){
		    					   isMoveEllipse=TRUE;
		    					   isMoveRectangle=FALSE;
		    					   isMoveLine=FALSE;
		    					    changenow=i;
		    					    lx=x1,ly=y1;
		    					    sx=mx,sy=my;
		    					    sx1=mx,sy1=my;
		    					    rx=x1-x2,ry=y1-y2;
		    					    break;
		    				    }
		    			    }
		    			    else if(onScreen[i].type==line){
		    				    if(inBox(mx, my, x1, x2, y1, y2)){		    
		    					    isMoveLine=TRUE;
		    					    isMoveRectangle=FALSE;
		    					    isMoveEllipse=FALSE;
		    					    changenow=i;
		    					    lx=x1,ly=y1;
		    					    sx=mx,sy=my;
		    					    sx1=mx,sy1=my;
		    					    rx=x2-x1,ry=y2-y1;
		    					    break;
		    				    }
		    			    }
						}
						int j=-1;
						for(i=0;i<textnum;i++){   /*�ж�ѡ����ı�*/
							x1=alltext[i].textx;
						    x2=alltext[i].textx1;
						    y1=alltext[i].texty;
						    y2=alltext[i].texty1;
						    if(inBox(mx,my,x1,x2,y1,y2)){
						    	textnow = i;
						    	j = i;
						    	startDrawText = TRUE;
						    	isMoveText = TRUE;
						    	sx = mx;
						    	sy = my;
						    }
						}
						gx = alltext[textnow].textx1;
						gy = alltext[textnow].texty;
						if(j==-1)startDrawText = FALSE;
			        }else if(isdelete) {       /*�ж�Ҫɾ����ͼ��*/
		    		    int i;
		    		    double x1,x2,y1,y2;
		    		    for(i=0;i<num;i++){
		    			    x1=onScreen[i].x1;
						    x2=onScreen[i].x2;
						    y1=onScreen[i].y1;
						    y2=onScreen[i].y2;
						    
						    if(onScreen[i].type==rectangle){
		    				    if(inBox(mx,my,x1,x2,y1,y2)){
						   		    isMoveRectangle=TRUE;
						   		    isMoveLine=FALSE;
						   		    isMoveEllipse=FALSE;
						   		    startdelete=TRUE;
						   		    changenow=i;
						   		    lx=x1,ly=y1;
						   		    sx=mx,sy=my;
						   		    sx1=mx,sy1=my;
						   		    rx=x2-x1,ry=y1-y2;
						   		    break;
							    }
						    }
					 		else if(onScreen[i].type==ellipse){
		    				    if(inBox(mx,my,x1,x2,y1,y2)){
		    					   isMoveEllipse=TRUE;
		    					   isMoveRectangle=FALSE;
		    					   isMoveLine=FALSE;
		    					   startdelete=TRUE;
		    					    changenow=i;
		    					    lx=x1,ly=y1;
		    					    sx=mx,sy=my;
		    					    sx1=mx,sy1=my;
		    					    rx=x1-x2,ry=y1-y2;
		    					    break;
		    				    }
		    			    }
		    			    else if(onScreen[i].type==line){
		    				    if(inBox(mx, my, x1, x2, y1, y2)){		    
		    					    isMoveLine=TRUE;
		    					    isMoveRectangle=FALSE;
		    					    isMoveEllipse=FALSE;
		    					    startdelete=TRUE;
		    					    changenow=i;
		    					    lx=x1,ly=y1;
		    					    sx=mx,sy=my;
		    					    sx1=mx,sy1=my;
		    					    rx=x2-x1,ry=y2-y1;
		    					    break;
		    				    }
		    			    }
						}
						int j=-1;
						for(i=0;i<textnum;i++){   /*�ж�ѡ����ı�*/
							x1=alltext[i].textx;
						    x2=alltext[i].textx1;
						    y1=alltext[i].texty;
						    y2=alltext[i].texty1;
						    if(inBox(mx,my,x1,x2,y1,y2)){
						    	textnow = i;
						    	j = i;
						    	startDeleteText = TRUE;
						    }
						}
						gx = alltext[textnow].textx1;
						gy = alltext[textnow].texty;
						if(j==-1)startDrawText = FALSE;
			        }else if(isDrawLine){
			        	startDrawLine = TRUE;
			        	sx=mx,sy=my;
			        	sx1=mx,sy1=my;
			        }else if(isDrawRectangle){
			        	startDrawRectangle = TRUE;
			        	sx=mx,sy=my;
			        	sx1=mx,sy1=my;
			        }else if(isDrawEllipse){
			        	startDrawEllipse = TRUE;
			        	sx=mx,sy=my;
			        	sx1=mx,sy1=my;
			        }
			    }
 	 		        break;
			  
					
				 
				case RIGHT_BUTTON:
					/*�Ҽ�ѡ��ͼ�ε�׼������*/
					if(ischoose) {
		    		    int i;
		    		    double x1,x2,y1,y2;
		    		    for(i=0;i<num;i++){
		    			    x1=onScreen[i].x1;
						    x2=onScreen[i].x2;
						    y1=onScreen[i].y1;
						    y2=onScreen[i].y2;
						    
						    if(onScreen[i].type==rectangle){
		    				    if(inBox(mx,my,x1,x2,y1,y2)){
						   		    changeRectangle=TRUE;
						   		    changeLine=FALSE;
						   		    changeEllipse=FALSE;
						   		    changenow=i;
						   		    lx=x1,ly=y1;
						   		    sx=mx,sy=my;
						   		    sx1=mx,sy1=my;
						   		    rx=x2-x1,ry=y2-y1;
						   		    break;
							    }
						    }
					 		else if(onScreen[i].type==ellipse){
		    				    if(inBox(mx,my,x1,x2,y1,y2)){
		    					    changeEllipse=TRUE;
		    					    changeLine=FALSE;
		    					    changeRectangle=FALSE;
		    					    changenow=i;
		    					    lx=x1,ly=y1;
		    					    sx=mx,sy=my;
		    					    sx1=mx,sy1=my;
		    					    rx=x1-x2,ry=y1-y2;
		    					    break;
		    				    }
		    			    }
		    			    else if(onScreen[i].type==line){
		    				    if(inBox(mx, my, x1, x2, y1, y2)){		    
		    					    changeLine=TRUE;
		    					    changeRectangle=FALSE;
		    					    changeEllipse=FALSE;
		    					    changenow=i;
		    					    lx=x1,ly=y1;
		    					    sx=mx,sy=my;
		    					    sx1=mx,sy1=my;
		    					    rx=x2-x1,ry=y2-y1;
		    					    break;
		    				    }
		    			    }
						}
			        }
			        
 	 		        break;
 	    }break;
 	 	case BUTTON_UP:
 	 		
 	 		if(startDrawLine && (mx!=sx1 || my!=sy1)){ /*���ͼ�νṹ*/
 	 			onScreen[num].type = line;
 	 			onScreen[num].x1 = sx1;
 	 			onScreen[num].y1 = sy1;
 	 			onScreen[num].x2 = mx;
 	 			onScreen[num].y2 = my;
 	 			num++;
 	 		}else if(startDrawRectangle && (mx!=sx1 || my!=sy1)){
 	 			onScreen[num].type = rectangle;
 	 			onScreen[num].x1 = sx1;
 	 			onScreen[num].y1 = sy1;
 	 			onScreen[num].x2 = mx;
 	 			onScreen[num].y2 = my;
 	 			num++;
 	 		}else if(startDrawEllipse && (mx!=sx1 || my!=sy1)){
 	 			onScreen[num].type = ellipse;
 	 			onScreen[num].x1 = 2*sx1-mx;
 	 			onScreen[num].y1 = 2*sy1-my;
 	 			onScreen[num].x2 = mx;
 	 			onScreen[num].y2 = my;
 	 			num++;
 	 		}
 	 		if(startdelete){  /*ɾ��ͼ��*/
 	 			int k;
 	 			k = changenow;
 	 			switch(onScreen[k].type){
 	 				case line:SetEraseMode(TRUE);
			                  MovePen(onScreen[k].x1,onScreen[k].y1);
                              DrawLine(onScreen[k].x2-onScreen[k].x1,onScreen[k].y2-onScreen[k].y1);
                              SetEraseMode(FALSE);
                 	          int j;
                 	          for(j=k;j<num-1;j++)
                 	          {
                 	 	          onScreen[j].x1 = onScreen[j+1].x1;
                 	 	          onScreen[j].x2 = onScreen[j+1].x2;
                 	 	          onScreen[j].y1 = onScreen[j+1].y1;
                 	 	          onScreen[j].y2 = onScreen[j+1].y2;
                 	 	          onScreen[j].type = onScreen[j+1].type;
                 	          }
                 	          num--;
                 	          break;
                 	case rectangle:SetEraseMode(TRUE);
			                       MovePen(onScreen[k].x1,onScreen[k].y1);
                                   DrawRectangle(onScreen[k].x2-onScreen[k].x1,onScreen[k].y1-onScreen[k].y2);
                                   SetEraseMode(FALSE);
                 	               int m;
                 	               for(m=k;m<num-1;m++)
                 	               {
                 	 	               onScreen[m].x1 = onScreen[m+1].x1;
                 	 	               onScreen[m].x2 = onScreen[m+1].x2;
                 	 	               onScreen[m].y1 = onScreen[m+1].y1;
                 	 	               onScreen[m].y2 = onScreen[m+1].y2;
                 	 	               onScreen[m].type = onScreen[m+1].type;
                 	               }
                 	               num--;
                 	               break;
                 	case ellipse:SetEraseMode(TRUE);
			                     MovePen(onScreen[k].x2,(onScreen[k].y1+onScreen[k].y2)/2.0);
                                 DrawEllipticalArc((onScreen[k].x2-onScreen[k].x1)/2.0,(onScreen[k].y2-onScreen[k].y1)/2.0,0,360);
                                 SetEraseMode(FALSE);
                 	             int n;
                 	             for(n=k;j<num-1;n++)
                 	             {
                 	 	             onScreen[n].x1 = onScreen[n+1].x1;
                 	 	             onScreen[n].x2 = onScreen[n+1].x2;
                 	 	             onScreen[n].y1 = onScreen[n+1].y1;
                 	 	             onScreen[n].y2 = onScreen[n+1].y2;
                 	 	             onScreen[n].type = onScreen[n+1].type;
                 	             }
                 	              num--;
                 	             break;
 	 			}
 	 		}
 	 		if(startDeleteText){
 	 			int i = textnow;
 	 			SetEraseMode(TRUE);
 	 			MovePen(alltext[i].textx,alltext[i].texty1);
 	 			char * p;
 	 			p = &(alltext[i].texts[0]);
 	 			DrawTextString(p);
 	 			SetEraseMode(FALSE);
 	 			int j;
 	 			for(j=i;j<textnum;j++){
 	 				alltext[j].textx = alltext[j+1].textx;
 	 				alltext[j].texty = alltext[j+1].texty;
 	 				alltext[j].textx1 = alltext[j+1].textx1;
 	 				alltext[j].texty1 = alltext[j+1].texty1;
 	 				alltext[j].current = alltext[j+1].current;
 	 				int k;
 	 				for(k=0;k<=alltext[j+1].textlen;k++){
 	 					alltext[j].texts[k] = alltext[j+1].texts[k];
 	 				}
 	 				alltext[j].textlen = alltext[j+1].textlen;
 	 			}
 	 			textnum--;
 	 		}
 	 		startDrawLine = FALSE;   /*״̬��ԭ*/
 	 		startDrawRectangle = FALSE;
 	 		startDrawEllipse = FALSE;
 	 		startdelete = FALSE;
 	 		startDeleteText = FALSE;
 	 		isMoveLine = FALSE;
 	 		isMoveRectangle = FALSE;
 	 		isMoveEllipse = FALSE;
 	 		isMoveText = FALSE;
 	 		changeLine = FALSE;
 	 		changeRectangle = FALSE;
 	 		changeEllipse = FALSE;
 	 		
 	 		break;
 	 		
 	 	case MOUSEMOVE:
 	 		
 	 		if(isDrawText)break;
 	 		/*��ͼ*/
 	 		if(startDrawLine){
 	 			SetEraseMode(TRUE);/*����ǰһ��*/
 	 			MovePen(sx1,sy1);
                  DrawLine(sx-sx1,sy-sy1);
                  sx=mx;
                  sy=my;
				  SetEraseMode(FALSE);/*���µ�*/
				  MovePen(sx1,sy1);
                  DrawLine(sx-sx1,sy-sy1);
 	 		}
 	 		else if(startDrawRectangle){
 	 			SetEraseMode(TRUE);
 	 			MovePen(sx1,sy1);
 	 			DrawRectangle(sx-sx1,sy1-sy);
 	 			sx=mx;
 	 			sy=my;
 	 			SetEraseMode(FALSE);
 	 			MovePen(sx1,sy1);
 	 			DrawRectangle(sx-sx1,sy1-sy);
 	 		}
 	 		else if(startDrawEllipse){
 	 			SetEraseMode(TRUE);
 	 			MovePen(2*sx1-sx,sy1);
 	 			DrawEllipticalArc(sx1-sx,sy1-sy,0,360);
 	 			sx=mx;
 	 			sy=my;
 	 			SetEraseMode(FALSE);
 	 			MovePen(2*sx1-sx,sy1);
 	 			DrawEllipticalArc(sx1-sx,sy1-sy,0,360);
 	 		}
 	 		/*�ƶ�*/
 	 		else  if (isMoveEllipse) {
 	 			int i=changenow;
 	 			double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;
                  SetEraseMode(TRUE);
                  MovePen(x2,(y1+y2)/2);
                  DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
				  x1 += mx - sx;
				  y1 += my - sy;
				  x2 += mx - sx;
				  y2 += my - sy;
				  onScreen[i].x1 = x1;
				  onScreen[i].y1 = y1;
				  onScreen[i].x2 = x2;
				  onScreen[i].y2 = y2;
				  sx = mx;
				  sy = my;
				  SetEraseMode(FALSE);
				  MovePen(x2,(y1+y2)/2);
                  DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
                  	
			  }
 	 		else if (isMoveRectangle) {
 	 			int i=changenow;
 	 			double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;
                  SetEraseMode(TRUE);
                  MovePen(lx,ly);
                  DrawRectangle(rx,ry);
				  lx += mx - sx;
				  ly += my - sy;
				  onScreen[i].x1 = lx;
				  onScreen[i].y1 = ly;
				  onScreen[i].x2 = x2 + mx - sx;
				  onScreen[i].y2 = y2 + my - sy;
				  sx = mx;
				  sy = my;
				  SetEraseMode(FALSE);
				  MovePen(lx,ly);
                  DrawRectangle(rx,ry);
			  }
 	 		else if(isMoveLine) {
 	 			int i=changenow;
 	 			double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;
                  SetEraseMode(TRUE);
                  MovePen(lx,ly);
                  DrawLine(rx,ry);
				  lx += mx - sx;
				  ly += my - sy;
				  onScreen[i].x1 = lx;
				  onScreen[i].y1 = ly;
				  onScreen[i].x2 = x2 + mx - sx;
				  onScreen[i].y2 = y2 + my - sy;
				  sx = mx;
				  sy = my;
				  SetEraseMode(FALSE);
				  MovePen(lx,ly);
                  DrawLine(rx,ry);
			  }
			  else if(isMoveText){
			  	  int i = textnow;
			  	  SetEraseMode(TRUE);
			  	  MovePen(alltext[i].textx,alltext[i].texty1);
			  	  char *p;
			  	  p = &(alltext[i].texts[0]);
			  	  DrawTextString(p);
			  	  MovePen(gx,gy);
			  	  DrawLine(0,-GetFontHeight());
			  	  alltext[i].textx += mx-sx;
			  	  alltext[i].texty += my-sy;
			  	  alltext[i].textx1 += mx-sx;
			  	  alltext[i].texty1 += my-sy;
			  	  gx += mx-sx;
			  	  gy += my-sy; 
			  	  sx = mx;
			  	  sy = my;
			  	  SetEraseMode(FALSE);
			  	  MovePen(alltext[i].textx,alltext[i].texty1);
			  	  DrawTextString(p);
			  }
 	 		
 	 		/*��С�ı�*/
 	 		else if(changeLine){
 	 			int i=changenow;
 	 			double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;
 	 			SetEraseMode(TRUE);
 	 			MovePen(lx,ly);
 	 			DrawLine(onScreen[i].x2-lx,onScreen[i].y2-ly);
 	 			onScreen[i].x2 = mx;
 	 			onScreen[i].y2 = my;
 	 			sx=mx,sy=my;
 	 			SetEraseMode(FALSE);
                MovePen(lx,ly);
                DrawLine(onScreen[i].x2-lx,onScreen[i].y2-ly);			  
 	 		}
 	 		else if(changeRectangle){
 	 			int i=changenow;
 	 			double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;
 	 			SetEraseMode(TRUE);
 	 			MovePen(onScreen[i].x1,onScreen[i].y1);
 	 			DrawRectangle(onScreen[i].x2-lx,ly-onScreen[i].y2);
 	 			SetEraseMode(FALSE);
 	 			onScreen[i].x2=mx;
 	 			onScreen[i].y2=my;
 	 			sx=mx;
 	 			sy=my;
 	 			MovePen(onScreen[i].x1,onScreen[i].y1);
 	 			DrawRectangle(onScreen[i].x2-lx,ly-onScreen[i].y2);
 	 		}
 	 		else if(changeEllipse){
 	 			int i=changenow;
 	 			double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;
 	 			SetEraseMode(TRUE);
 	 			MovePen(x2,(y1+y2)/2.0);
 	 			DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
 	 			SetEraseMode(FALSE);
 	 			x2=mx;
 	 			y2=my;
 	 			onScreen[i].x2=x2;
 	 			onScreen[i].y2=y2;
 	 			MovePen(x2,(y1+y2)/2.0);
 	 		    DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);	
 	 		}	
		    if(startDrawLine || startDrawRectangle || startDrawEllipse || isMoveLine || isMoveRectangle || isMoveEllipse || changeLine
			|| changeRectangle || changeEllipse)
			{double x1,x2,y1,y2;
 	 		int j;
		    		    for(j=0;j<num;j++){
		    			    x1=onScreen[j].x1;
						    x2=onScreen[j].x2;
						    y1=onScreen[j].y1;
						    y2=onScreen[j].y2;
						    
						    if(onScreen[j].type==rectangle){
		    				    SetEraseMode(TRUE);
		    				    MovePen(x1,y1);
		    				    DrawRectangle(x2-x1,y1-y2);
		    				    SetEraseMode(FALSE);
		    				    MovePen(x1,y1);
		    				    DrawRectangle(x2-x1,y1-y2);
						    }
					 		else if(onScreen[j].type==ellipse){
		    				    SetEraseMode(TRUE);
		    				    MovePen(x2,(y1+y2)/2.0);
		    				    DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
		    				    SetEraseMode(FALSE);
		    				    MovePen(x2,(y1+y2)/2.0);
		    				    DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
		    			    }
		    			    else if(onScreen[j].type==line){
		    				    SetEraseMode(TRUE);
		    				    MovePen(x1,y1);
		    				    DrawLine(x2-x1,y2-y1);
		    				    SetEraseMode(FALSE);
		    				    MovePen(x1,y1);
		    				    DrawLine(x2-x1,y2-y1);
		    			    }
						}
					}
					int b;
					char *p;
					for(b=0;b<textnum;b++)
					{
						p = &(alltext[b].texts[0]);
						MovePen(alltext[b].textx,alltext[b].texty1);
						SetEraseMode(TRUE);
						DrawTextString(p);
						MovePen(alltext[b].textx,alltext[b].texty1);
						SetEraseMode(FALSE);
						DrawTextString(p);
					}
					SetEraseMode(FALSE);
					Layout();
 	 		break;
 	 		
 	 }
}

void KeyboardEventProcess(int key,int event)
{
     int i,j;
	 i = changenow;
     j = textnow;
	  	 
 	 if(startDrawText){  /*�ı�����ƶ���ɾ��*/
	static char str[2] = {0, 0};
	int k = alltext[j].current;
	SetEraseMode(TRUE);
	 	   MovePen(gx,gy);
	 	   DrawLine(0,-GetFontHeight());
	 	   SetEraseMode(FALSE);
	switch (event) {
	 	case KEY_DOWN:
			 switch (key) {
			     case VK_LEFT:
			         if(k>0)
			         {
			         	str[0] = alltext[j].texts[k-1];
			         	gx -= TextStringWidth(str);
			         	alltext[j].current--;
			         }
                     break;
			     case VK_RIGHT:
			         if(k<alltext[j].textlen)
			         {
			         	str[0] = alltext[j].texts[k];
			         	gx += TextStringWidth(str);
			         	alltext[j].current++;
			         }
                     break;
                 case VK_BACK:
                 	 if(k>0)
                 	 {
                 	 	SetEraseMode(TRUE);
                 	 	char *p;
                 	 	p = &(alltext[j].texts[k-1]);
                 	 	str[0] = alltext[j].texts[k-1];
                 	 	gx -= TextStringWidth(str);
                 	 	
                 	 	MovePen(gx,gy-GetFontHeight());
                 	 	DrawTextString(p);
                 	 	
                 	 	int m;
                 	 	for(m=k-1;m<alltext[j].textlen;m++)
                 	 	{
                 	 		alltext[j].texts[m] = alltext[j].texts[m+1];
                 	 	}
                 	 	alltext[j].textlen--;
                 	 	alltext[j].current--;
                 	 	k = alltext[j].current;
                 	 	p = &(alltext[j].texts[k]);
                 	 	SetEraseMode(FALSE);
                 	 	
                 	 	MovePen(gx,gy-GetFontHeight());
                 	 	DrawTextString(p);
                 	 	alltext[j].textx1 -= TextStringWidth(str);
                 	 }
                 	 break;
			 }
			 break;
		case KEY_UP:
			 break;
   }
   }else if(onScreen[i].type == line){  /*ͼ���ƶ���ɾ��*/
 	 
     switch (event) {
	 	case KEY_DOWN:
			 switch (key) {
			     case VK_UP:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
					 onScreen[i].y1 += deltay;
					 onScreen[i].y2 += deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
                     break;
			     case VK_DOWN:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
					 onScreen[i].y1 -= deltay;
					 onScreen[i].y2 -= deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
                     break;
			     case VK_LEFT:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
					 onScreen[i].x1 -= deltay;
					 onScreen[i].x2 -= deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
                     break;
			     case VK_RIGHT:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
					 onScreen[i].x1 += deltay;
					 onScreen[i].x2 += deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
                     break;
                 case VK_DELETE:
                 	 SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);
                     SetEraseMode(FALSE);
                 	 int j = i;
                 	 for(j=i;j<num-1;j++)
                 	 {
                 	 	onScreen[j].x1 = onScreen[j+1].x1;
                 	 	onScreen[j].x2 = onScreen[j+1].x2;
                 	 	onScreen[j].y1 = onScreen[j+1].y1;
                 	 	onScreen[j].y2 = onScreen[j+1].y2;
                 	 	onScreen[j].type = onScreen[j+1].type;
                 	 }
                 	 num--;
                 	 break;
			 }
			 break;
		case KEY_UP:
			 break;
	 }	 
   }else if(onScreen[i].type == rectangle){
   	    switch (event) {
	 	case KEY_DOWN:
			 switch (key) {
			     case VK_UP:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
					 onScreen[i].y1 += deltay;
					 onScreen[i].y2 += deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
                     break;
			     case VK_DOWN:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
					 onScreen[i].y1 -= deltay;
					 onScreen[i].y2 -= deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
                     break;
			     case VK_LEFT:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
					 onScreen[i].x1 -= deltay;
					 onScreen[i].x2 -= deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
                     break;
			     case VK_RIGHT:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
					 onScreen[i].x1 += deltay;
					 onScreen[i].x2 += deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
                     break;
                 case VK_DELETE:
                 	 SetEraseMode(TRUE);
			         MovePen(onScreen[i].x1,onScreen[i].y1);
                     DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);
                     SetEraseMode(FALSE);
                 	 int j = i;
                 	 for(j=i;j<num-1;j++)
                 	 {
                 	 	onScreen[j].x1 = onScreen[j+1].x1;
                 	 	onScreen[j].x2 = onScreen[j+1].x2;
                 	 	onScreen[j].y1 = onScreen[j+1].y1;
                 	 	onScreen[j].y2 = onScreen[j+1].y2;
                 	 	onScreen[j].type = onScreen[j+1].type;
                 	 }
                 	 num--;
                 	 break;
			 }
			 break;
		case KEY_UP:
			 break;
   }
}else if(onScreen[i].type == ellipse){
   	    switch (event) {
	 	case KEY_DOWN:
			 switch (key) {
			     case VK_UP:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
					 onScreen[i].y1 += deltay;
					 onScreen[i].y2 += deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
                     break;
			     case VK_DOWN:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
					 onScreen[i].y1 -= deltay;
					 onScreen[i].y2 -= deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
                     break;
			     case VK_LEFT:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
					 onScreen[i].x1 -= deltay;
					 onScreen[i].x2 -= deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
                     break;
			     case VK_RIGHT:
			         SetEraseMode(TRUE);
			         MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
					 onScreen[i].x1 += deltay;
					 onScreen[i].x2 += deltay;
					 SetEraseMode(FALSE);
                     MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
                     break;
                 case VK_DELETE:
                 	 SetEraseMode(TRUE);
			         MovePen(onScreen[i].x2,(onScreen[i].y1+onScreen[i].y2)/2.0);
                     DrawEllipticalArc((onScreen[i].x2-onScreen[i].x1)/2.0,(onScreen[i].y2-onScreen[i].y1)/2.0,0,360);
                     SetEraseMode(FALSE);
                 	 int j = i;
                 	 for(j=i;j<num-1;j++)
                 	 {
                 	 	onScreen[j].x1 = onScreen[j+1].x1;
                 	 	onScreen[j].x2 = onScreen[j+1].x2;
                 	 	onScreen[j].y1 = onScreen[j+1].y1;
                 	 	onScreen[j].y2 = onScreen[j+1].y2;
                 	 	onScreen[j].type = onScreen[j+1].type;
                 	 }
                 	 num--;
                 	 break;
			 }
			 break;
		case KEY_UP:
			 break;
   }

}
                        double x1,x2,y1,y2;  /*��ԭ��������ͼ��*/
 	 		            int a;
		    		    for(a=0;a<num;a++){
		    			    x1=onScreen[a].x1;
						    x2=onScreen[a].x2;
						    y1=onScreen[a].y1;
						    y2=onScreen[a].y2;
						    
						    if(onScreen[a].type==rectangle){
		    				    SetEraseMode(TRUE);
		    				    MovePen(x1,y1);
		    				    DrawRectangle(x2-x1,y1-y2);
		    				    SetEraseMode(FALSE);
		    				    MovePen(x1,y1);
		    				    DrawRectangle(x2-x1,y1-y2);
						    }
					 		else if(onScreen[a].type==ellipse){
		    				    SetEraseMode(TRUE);
		    				    MovePen(x2,(y1+y2)/2.0);
		    				    DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
		    				    SetEraseMode(FALSE);
		    				    MovePen(x2,(y1+y2)/2.0);
		    				    DrawEllipticalArc((x2-x1)/2.0,(y2-y1)/2.0,0,360);
		    			    }
		    			    else if(onScreen[a].type==line){
		    				    SetEraseMode(TRUE);
		    				    MovePen(x1,y1);
		    				    DrawLine(x2-x1,y2-y1);
		    				    SetEraseMode(FALSE);
		    				    MovePen(x1,y1);
		    				    DrawLine(x2-x1,y2-y1);
		    			    }
						}
						int b;
						char *p;
					for(b=0;b<textnum;b++)
					{
						p = &(alltext[b].texts[0]);
						MovePen(alltext[b].textx,alltext[b].texty1);
						SetEraseMode(TRUE);
						DrawTextString(p);
						MovePen(alltext[b].textx,alltext[b].texty1);
						SetEraseMode(FALSE);
						DrawTextString(p);
					}
						SetEraseMode(FALSE);
				    	Layout();
					
}

void CharEventProcess(char c)
{
	int j;
	j =  textnow;
 	 if(startDrawText){
 	 	
 	 SetEraseMode(FALSE);	
 	 	
 	 static char str[2] = {0, 0};
     switch (c) {
       case '\r':  /* ע�⣺�س������ﷵ�ص��ַ���'\r'������'\n'*/
           
           startDrawText = FALSE;
           SetEraseMode(TRUE);
	 	   MovePen(gx,gy);
	 	   DrawLine(0,-GetFontHeight());
	 	   SetEraseMode(FALSE);
   	 	   break;
   	   case 8:  /*Backspace*/
 	   case 27: /*ESC*/
 	       break;
      default:
      	   SetEraseMode(TRUE);
	 	   MovePen(gx,gy);
	 	   DrawLine(0,-GetFontHeight());
	 	   SetEraseMode(FALSE);
	 	   int k;
	 	   k = alltext[j].current;
	 	   char *p;
	 	   p = &(alltext[j].texts[k]);
	 	   SetEraseMode(TRUE);
	 	   MovePen(gx,gy-GetFontHeight());
		   DrawTextString(p);
		   
		   int m;
		   for(m=alltext[j].textlen;m>k;m--)
		   {
		   	  alltext[j].texts[m] = alltext[j].texts[m-1];
		   }	 	   
	 	   alltext[j].textlen++;
	 	   int n = alltext[j].textlen;
	 	   alltext[j].texts[n] = '\0';
	 	   alltext[j].texts[k] = c;
	 	   p = &(alltext[j].texts[k]);
	 	   MovePen(gx,gy-GetFontHeight());
	 	   SetEraseMode(FALSE);
	 	   DrawTextString(p);
	 	   str[0]=c;
	 	   gx+=TextStringWidth(str);
	 	   alltext[j].current++;
	 	   alltext[j].textx1+= TextStringWidth(str);
	 	   break;
        }
    }
}

void TimerEventProcess(int timerID)
{
	 timerID = TIMER_BLINK500;
	 if(startDrawText||isMoveText){
	 	bool isblink;
	 	isblink = GetEraseMode();
	 	SetEraseMode(isblink);
	 	MovePen(gx,gy);
	 	DrawLine(0,-GetFontHeight());
	 	isblink = !isblink;
	 	SetEraseMode(isblink);
	 }
}
