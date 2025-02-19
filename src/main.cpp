#include "IRRemote.hpp"
#include "Log.h"
#include "Timer.h"
#include <CircularBuffer.hpp>
#include <M5StickCPlus.h>

// テキストサイズ
#define TEXT_SIZE 1
// 画面の向き 1:横向き（USBポートが右） 3:横向き（USBポートが左）
// 2:縦向き（USBポートが下） 4:縦向き（USBポートが上）7
#define SCREAN_ROTATION 3
// 画面の幅
#define SCREEN_WIDTH 240
// 画面の高さ
#define SCREEN_HEIGHT 135

// 1行の高さ（フォントサイズに応じて調整）
#define SCROLL_LINE_HEIGHT 12
// スクロール領域の余白
#define SCREEN_SCROLL_MARGIN 5
// スクロール領域の開始位置 X軸
#define SCREEN_SCROLL_X 0
// スクロール領域の開始位置 Y軸
#define SCREEN_SCROLL_Y (SCROLL_LINE_HEIGHT)
// スクロール領域の幅
#define SCREEN_SCROLL_WIDTH (SCREEN_WIDTH - SCREEN_SCROLL_X)
// スクロール領域の高さ
#define SCREEN_SCROLL_HEIGHT (SCREEN_HEIGHT - SCREEN_SCROLL_Y)
// 最大行数
#define MAX_LINES                                                              \
  ((SCREEN_SCROLL_HEIGHT - SCREEN_SCROLL_MARGIN * 2) / SCROLL_LINE_HEIGHT)

// 赤外線リモコン通信インスタンス
IRRemote2 *irr;

// 新しいテキストを表示し、スクロール処理を行う関数
void addScrollingText(String text) {
  // 文字列のバッファ
  static CircularBuffer<String, MAX_LINES> lines;

  if (lines.isFull()) {
    lines.shift();
  }
  lines.push(text);

  // 新しいテキストを描画
  M5.Lcd.fillRect(SCREEN_SCROLL_X, SCREEN_SCROLL_Y, SCREEN_SCROLL_WIDTH,
                  SCREEN_SCROLL_HEIGHT, BLACK);
  uint16_t currentY = SCREEN_SCROLL_Y + SCREEN_SCROLL_MARGIN;
  for (uint16_t i = 0; i < lines.size(); i++) {
    M5.Lcd.setCursor(SCREEN_SCROLL_MARGIN, currentY);
    M5.Lcd.println(lines[i]);
    currentY += SCROLL_LINE_HEIGHT;
  }
}

void setup() {
  M5.begin();

  irr = new IRRemote2(IRRemote2::IR_LOW);
  delay(500);
  logger.info("M5StickC-plus Start\n");

  M5.Lcd.setTextSize(TEXT_SIZE);
  M5.Lcd.setRotation(SCREAN_ROTATION);
  M5.Lcd.setCursor(SCREEN_SCROLL_MARGIN, SCREEN_SCROLL_MARGIN);
  M5.Lcd.println(irr->getDebugHeaderString());
}

void loop() {
  static Timer samplingTimer(4);

  if (samplingTimer.isCycleTime()) {
    if (irr->recieveIR()) {
      addScrollingText(irr->getDebugRecordString());
    }
  }
}
