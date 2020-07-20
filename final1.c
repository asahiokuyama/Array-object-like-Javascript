// arrayobj.c
//
// 配列オブジェクトひな型
//
// C言語でJavascriptのような配列オブジェクト機能を作成してください
// 扱う要素は、long型とします
//
// プロトタイプ宣言が書かれている4つの関数の作成は必須です。
// main関数の中からの呼び出しの記述がうまく動作するように作成してください。
// Javascriptの配列データのほかのメソッド、shift(先頭要素の削除)、
// unshift(先頭への要素の追加)などは、加点対象にしますが、
// main関数の最後に、その関数を使って呼び出しを記述して、
// どのような表示結果になるかを、元からある呼び出し例にならって記述してください
//
// aobj_push(),aobj_pop()の返すポインタを使っていませんが、
// 機能を追加する場合には、意味のあるポインタを返すようにしてかまいません
// ただし、元のプログラムはそのまま動作しないといけません。
//
// グローバル変数は使わないようにしてください。
// また、static変数も使わないようにして、
// 複数のスレッドから同時に呼び出されても不具合が生じないようにしてください
// 提出するのは、作成した関数の記述を追加した、本プログラムarrayobj.cです
// コンパイラの種類にもよりますが、基本的に注意メッセージがでないようにしてください
// また、動作や変数、関数の説明も必ずコメントとして記述してください
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

// 必要なヘッダファイルはこの後に追加してください
//​
// ここに、自分で考えた構造体の定義を記述します
typedef struct _DATA {
  int value;
  struct _DATA *next;
} DATA;

struct aobj {
  pthread_mutex_t lock;
  DATA head;
};

//​
// 次の4つの関数は、プロトタイプ宣言は、そのまま変更せず、
// これらに合致するように関数を作成してください
//
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
//​
//
// ここに必要な関数を作成し、記述してください
// struct aobj *aobj_push(struct aobj *a, long item){
//   ....
//   ....
// }
// のように記述することになります
//
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

// 
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

//
// mainは次のとおりとし、そのままコンパイルし、実行できるようにします
// 例えば、実行例は次のようになります
//
// %./kadai
// 10
// 20
// 40
// 30 <- このあとctrl-Dを入力
// len = 4  <- 入れた個数が表示されます
// remove 30, len = 3  <- 最後の要素を削除した結果
// remove 40, len = 2  <- もう一度、最後の要素を削除した結果
// %
//
//
#define MAXINPUT 10
//​
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
