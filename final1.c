// arrayobj.c
//
// 配列オブジェクトひな型
//
// C言語でJavascriptのような配列オブジェクト機能を作成
// %./a.out
// 10
// 20
// 40
// 30 <- このあとctrl-Dを入力
// len = 4  <- 入れた個数が表示されます
// remove 30, len = 3  <- 最後の要素を削除した結果
// remove 40, len = 2  <- もう一度、最後の要素を削除した結果
// %

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

//構造体の定義
typedef struct _DATA {
  int value;
  struct _DATA *next;
} DATA;

struct aobj {
  pthread_mutex_t lock;
  DATA head;
};

// 初期化の関数のプロトタイプ宣言
struct aobj *aobj_init();
//
// 配列の末尾に要素を追加する関数のプロトタイプ宣言
// 追加する要素を第2引数に入れて関数を呼び出す
struct aobj *aobj_push(struct aobj *a, long item);
//
// 配列の末尾の要素を削除する関数のプロトタイプ宣言
// 削除される要素を第2引数に入れ、その後でその要素を削除する
struct aobj *aobj_pop(struct aobj *a, long *item);
//
// 要素の個数を返す関数のプロトタイプ宣言
// 個数を返せる場合は関数の値として1を返し、エラーの場合は0を返す
// 個数は第2引数に入れる
// 配列の要素の個数が0の場合には，関数の値として1が返り，個数には0が入る
int aobj_len(struct aobj *a, unsigned int *len);

struct aobj *aobj_init()
{
  struct aobj *o = (struct aobj *)malloc(sizeof(struct aobj));
  if (!o) exit(1);
  pthread_mutex_init(&o->lock, NULL);
  o->head.next = NULL;
  return o;
}

struct aobj *aobj_push(struct aobj *a, long item)
{
  DATA *p;
  pthread_mutex_lock(&a->lock);
  for (p = &a->head; p->next; p = p->next) ;
  p->next = (DATA *)malloc(sizeof(DATA));
  if (p->next == NULL) exit(1);
  p = p->next;
  p->value = item;
  pthread_mutex_unlock(&a->lock);

  return a;
}


struct aobj *aobj_pop(struct aobj *a, long *item)
{
  DATA *p;
  pthread_mutex_lock(&a->lock);
  p = &a->head;
  if (p->next == NULL) {
    *item = 0;
  }
  else {
    while (p->next->next) p = p->next;
    *item = p->next->value;
    free(p->next);
    p->next = NULL;
  }
  pthread_mutex_unlock(&a->lock);

  return a;
}

int aobj_len(struct aobj *a, unsigned int *len)
{
  int ret = 1;
  unsigned int length = 0;
  pthread_mutex_lock(&a->lock);
  for (DATA* p = a->head.next; p; p = p->next) {
    length++;
  }
  *len = length;
  pthread_mutex_unlock(&a->lock);
  
  return ret;
}

#define MAXINPUT 10

int main(){
  struct aobj *myobj;
  char input[MAXINPUT];
  char *ret;
  long num, lastone;
  unsigned int len;

  myobj = aobj_init();
  // 数値の入力の繰り返し
  for(;;){
    ret = fgets(input,MAXINPUT,stdin);
    if(ret == NULL){
      break;
    }
    ret = strchr(input,'\n');
    if(ret!=NULL){
      *ret = '\0';
    }else{
      while(getchar()!='\n');
    }
    num = strtol(input,NULL,10);
    aobj_push(myobj, num);
  }
  // 長さの表示
  aobj_len(myobj,&len);
  printf("len = %u\n",len);
  // 最後の要素の削除と長さの表示
  aobj_pop(myobj, &lastone);
  aobj_len(myobj,&len);
  printf("remove %ld, len = %u\n",lastone,len);
  // もう一度、最後の要素の削除と長さの表示
  aobj_pop(myobj, &lastone);
  aobj_len(myobj,&len);
  printf("remove %ld, len = %u\n",lastone,len);
  exit(0);
}
