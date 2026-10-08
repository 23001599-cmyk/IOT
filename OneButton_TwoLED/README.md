# Điều khiển hai LED bằng một nút nhấn (OneButton)

Dự án PlatformIO cho ESP32. Chỉ dùng **một nút nhấn** để điều khiển **hai LED**, nhận dạng ba thao tác: single click, double click và giữ nút. Nút nhấn đọc bằng thư viện [OneButton](https://github.com/mathertel/OneButton) (Matthias Hertel), LED điều khiển bằng thư viện tự viết `lib/LED/LED.h` (Nguyễn Anh Tuấn).

Dự án phát triển từ ví dụ `OneButton_Lib_Demo` của lớp "Phát triển ứng dụng IoT" (khoa Vật lý - HUS), mở rộng từ một LED lên hai LED.

## 1. Chức năng

Tại mọi thời điểm chỉ có **một LED được chọn** (LED đang được điều khiển). Khi khởi động, LED1 được chọn.

| Thao tác | Kết quả |
|---|---|
| Single click (bấm 1 lần) | Bật/tắt LED đang chọn. Nếu LED đang nháy thì dừng nháy và tắt |
| Double click (bấm 2 lần liên tiếp) | Chuyển sang điều khiển LED còn lại (LED1 ↔ LED2). Trạng thái của LED cũ được giữ nguyên |
| Giữ nút (mặc định hơn 2 giây) | LED đang chọn nháy liên tục, 200 ms đổi trạng thái một lần |

Ví dụ: chọn LED1, giữ nút để LED1 nháy, double click để chuyển sang LED2, single click để bật LED2. Khi đó LED1 vẫn nháy và LED2 sáng. Mỗi thao tác đều in một dòng ra Serial Monitor (115200 baud) để biết LED nào đang được chọn.

Nút nhấn được khử rung (debounce) bởi chính thư viện OneButton (mặc định 50 ms).

## 2. Phần cứng

Cần: một nút nhấn nhả, một LED ngoài, một điện trở 220 Ω (330 Ω cũng được), breadboard (test board) và dây nối. **LED1** là LED có sẵn trên board, **LED2** là LED mắc thêm bên ngoài, nút nhấn cũng mắc thêm bên ngoài.

| Env (`platformio.ini`) | Board | LED1 | LED2 (ngoài) | Nút nhấn (ngoài) |
|---|---|---|---|---|
| `esp32doit-devkit-v1` | ESP32 DevKit v1 | GPIO2 (LED xanh trên board, active HIGH) | GPIO4 | GPIO18 |
| `esp32c3_super_mini` | ESP32-C3 Super Mini | GPIO8 (LED xanh trên board, active LOW) | GPIO3 | GPIO4 |
| `esp32c3_zero` | ESP32-C3 Zero | GPIO4 (LED ngoài, active HIGH) | GPIO5 | GPIO6 |

Với ESP32-C3 Zero, LED trên board là WS2812 (GPIO10) nên không điều khiển bằng `digitalWrite` được. Vì vậy cả hai LED đều là LED ngoài.

### Sơ đồ nối dây

LED ngoài (LED2, và LED1 của C3 Zero), active HIGH:

```
GPIO --[ 220 Ω ]--|>|-- GND
                  LED (chân dài / anode về phía điện trở, chân ngắn / cathode nối GND)
```

Nút nhấn:

```
GPIO ----[ nút nhấn ]---- GND
```

Không cần điện trở ngoài cho nút nhấn, vì OneButton bật điện trở kéo lên nội (`INPUT_PULLUP`). Khi nhả nút chân ở mức HIGH, khi nhấn nút chân bị kéo xuống LOW (`BTN_ACT = LOW`).

Các chân được chọn để tránh các chân strapping (GPIO0, 2, 5, 12, 15 của ESP32 và GPIO2, 8, 9 của C3) cho LED2 và nút ngoài, nên cắm thêm linh kiện không ảnh hưởng đến việc khởi động/nạp. Có thể đổi sang chân khác bằng cách sửa `build_flags` trong `platformio.ini`.

## 3. Cấu trúc thư mục

```
.
├── platformio.ini      # 3 môi trường (env), khai báo chân GPIO, thư viện OneButton
├── src/main.cpp        # chương trình chính
└── lib/LED/LED.h       # thư viện LED tự viết (on, off, flip, blink, loop)
```

## 4. Cách hoạt động

### 4.1. `platformio.ini`

Chân GPIO và mức tích cực (active level) được truyền vào bằng `build_flags`:

| Flag | Ý nghĩa |
|---|---|
| `LED1_PIN`, `LED1_ACT` | Chân và mức active của LED1 |
| `LED2_PIN`, `LED2_ACT` | Chân và mức active của LED2 |
| `BTN_PIN`, `BTN_ACT` | Chân và mức active của nút nhấn |

Nhờ vậy `main.cpp` dùng chung cho mọi board, chỉ cần chọn env tương ứng. Thư viện OneButton được PlatformIO tự tải qua `lib_deps`.

### 4.2. `src/main.cpp`

```cpp
LED led1(LED1_PIN, LED1_ACT);
LED led2(LED2_PIN, LED2_ACT);
LED *const leds[] = {&led1, &led2};
uint8_t selected = 0;                      // 0 = LED1, 1 = LED2

OneButton button(BTN_PIN, !BTN_ACT);

void setup() {
    button.setPressMs(HOLD_MS);            // thời gian giữ nút (mặc định 2000 ms)
    button.attachClick(btnClick);
    button.attachDoubleClick(btnDoubleClick);
    button.attachLongPressStart(btnHold);
}

void loop() {
    led1.loop();                           // nháy LED không chặn (millis)
    led2.loop();
    button.tick();                         // đọc nút, khử rung, nhận dạng thao tác
}

void btnClick()       { leds[selected]->flip(); }
void btnDoubleClick() { selected = (selected + 1) % LED_COUNT; }
void btnHold()        { leds[selected]->blink(BLINK_MS); }
```

Ý tưởng chính: biến `selected` là chỉ số LED đang chọn, và con trỏ `leds[selected]` trỏ tới LED đó. Các hàm xử lý single click và giữ nút không cần biết đó là LED nào, double click chỉ việc đổi `selected`. Muốn thêm LED thứ ba thì chỉ cần khai báo thêm và đưa vào mảng `leds`.

Lưu ý:
- `button.tick()`, `led1.loop()`, `led2.loop()` phải được gọi thường xuyên trong `loop()`. Không dùng `delay()` để không bỏ lỡ thao tác bấm và để hai LED nháy độc lập được với nhau.
- OneButton cần chờ một khoảng ngắn (mặc định khoảng 400 ms) sau lần bấm đầu tiên để biết đó là single hay double click, nên single click có độ trễ nhỏ. Đây là hành vi bình thường.
- Muốn đổi thời gian giữ nút, thêm `'-D HOLD_MS=1000'` vào `build_flags`.

### 4.3. `lib/LED/LED.h`

Lớp `LED` là máy trạng thái nhỏ với 3 trạng thái `OFF`, `ON`, `BLINK`:
- `on()`, `off()`: bật/tắt LED, có tính đến mức tích cực của board.
- `flip()`: đảo trạng thái (nếu đang nháy thì tắt).
- `blink(ms)`: chuyển sang nháy với chu kỳ đổi trạng thái `ms`.
- `loop()`: ở trạng thái `BLINK`, so sánh `millis()` để đổi mức chân LED.

Thư viện này giữ nguyên, không sửa gì so với ví dụ gốc.

## 5. Build và nạp

Cần VS Code + PlatformIO IDE (hoặc PlatformIO Core).

1. Mở thư mục dự án bằng VS Code.
2. Chọn env đúng với board (thanh dưới cùng, hoặc dùng `-e` trên dòng lệnh).
3. Build, sau đó Upload:
   ```
   pio run -e esp32doit-devkit-v1 -t upload
   pio device monitor
   ```
4. Nếu board không nhận lần đầu: giữ nút BOOT, bấm RESET, rồi nạp lại.

## 6. Cách thử

1. Reset board. Serial in `Dang dieu khien: LED1`.
2. Single click: LED1 bật. Single click lần nữa: LED1 tắt.
3. Double click: Serial in `Dang dieu khien: LED2`. Single click: LED2 bật/tắt.
4. Giữ nút hơn 2 giây: LED đang chọn nháy mỗi 200 ms.
5. Double click để chuyển sang LED kia: LED vừa nháy vẫn tiếp tục nháy, LED mới chọn điều khiển độc lập.
6. Khi LED đang nháy, single click: LED đó tắt.

## 7. Ghi chú

- Phần mềm đã được kiểm tra logic bằng mô phỏng trên máy tính (giả lập hàm Arduino, thư viện OneButton thật) với các tình huống ở mục 6. Cần thử trên phần cứng thật để xác nhận chân GPIO và dây nối của từng board.
- Thư viện OneButton: https://github.com/mathertel/OneButton
