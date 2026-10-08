# Điều khiển LED bằng nút nhấn: single click và double click (OneButton)

Dự án PlatformIO cho ESP32, dùng thư viện [OneButton](https://github.com/mathertel/OneButton) (Matthias Hertel) để đọc nút bấm và thư viện LED tự viết (`lib/LED/LED.h`, tác giả Nguyễn Anh Tuấn) để điều khiển đèn.

Dự án được sửa từ ví dụ `OneButton_Lib_Demo` trên lớp "Phát triển ứng dụng IoT" (khoa Vật lý - HUS).

## 1. Chức năng

| Thao tác | Kết quả |
|---|---|
| Bấm 1 lần (single click) | Đảo trạng thái LED: đang tắt thì bật, đang bật thì tắt |
| Bấm 2 lần liên tiếp (double click) | LED chuyển sang nháy liên tục, 200 ms đổi trạng thái một lần |
| Bấm 1 lần khi LED đang nháy | LED dừng nháy và tắt. Bấm tiếp thì bật/tắt như bình thường |

Khác với ví dụ gốc: ví dụ gốc giữ nút hơn 2 giây để nháy LED, dự án này dùng **double click** thay cho thao tác giữ nút. Chức năng bật/tắt bằng single click giữ nguyên.

Nút bấm được khử rung (debounce) bởi chính thư viện OneButton.

## 2. Phần cứng

Chỉ cần một nút bấm và một LED. Mặc định dùng nút BOOT và LED có sẵn trên board, riêng ESP32-C3 Zero dùng LED ngoài.

| Env (platformio.ini) | Board | Nút | LED |
|---|---|---|---|
| `esp32doit-devkit-v1` | ESP32 DevKit v1 | GPIO0 (BOOT), active LOW | GPIO2 (LED xanh), active HIGH |
| `esp32c3_super_mini` | ESP32-C3 Super Mini | GPIO9 (BOOT), active LOW | GPIO8 (LED xanh), active LOW |
| `esp32c3_zero` | ESP32-C3 Zero | GPIO9 (BOOT), active LOW | GPIO4 (LED ngoài), active HIGH |

Với ESP32-C3 Zero, LED trên board là LED RGB WS2812 (GPIO10) nên không điều khiển bằng `digitalWrite` thông thường. Vì vậy dùng một LED ngoài, mắc như sau:

```
GPIO4 --[ 220-330 ohm ]--|>|-- GND
                         LED (chân dài/anode phía điện trở, chân ngắn/cathode nối GND)
```

## 3. Cấu trúc thư mục

```
.
├── platformio.ini      # khai báo 3 môi trường (env), thư viện OneButton
├── src/main.cpp        # chương trình chính
└── lib/LED/LED.h       # thư viện LED tự viết (on, off, flip, blink, loop)
```

## 4. Cách hoạt động

### 4.1. `platformio.ini`
Chân nút, chân LED và mức tích cực (active level) được truyền vào bằng `build_flags` (`BTN_PIN`, `BTN_ACT`, `LED_PIN`, `LED_ACT`). Nhờ vậy `main.cpp` dùng chung cho mọi board, chỉ cần chọn env tương ứng. Thư viện OneButton được PlatformIO tự tải về qua `lib_deps`.

### 4.2. `src/main.cpp`
```cpp
LED led(LED_PIN, LED_ACT);
OneButton button(BTN_PIN, !BTN_ACT);   // tham số 2: mức logic khi nút KHÔNG bấm

void setup() {
    led.off();
    button.attachClick(btnClick);              // single click
    button.attachDoubleClick(btnDoubleClick);  // double click
}

void loop() {
    led.loop();      // xử lý nháy LED không chặn (dùng millis)
    button.tick();   // đọc nút, khử rung, nhận dạng click
}

void btnClick()       { led.flip(); }
void btnDoubleClick() { led.blink(200); }
```
- `button.tick()` phải được gọi thường xuyên trong `loop()`, không dùng `delay()` để không bỏ lỡ thao tác bấm.
- OneButton cần chờ một khoảng ngắn (mặc định khoảng 400 ms) sau lần bấm đầu để biết đó là single hay double click. Vì vậy single click có độ trễ nhỏ, đây là hành vi bình thường.

### 4.3. `lib/LED/LED.h`
Lớp `LED` là một máy trạng thái nhỏ với 3 trạng thái `OFF`, `ON`, `BLINK`:
- `on()`, `off()`: bật/tắt LED, có tính đến mức tích cực của board.
- `flip()`: đảo trạng thái (nếu đang nháy thì tắt).
- `blink(ms)`: chuyển sang nháy với chu kỳ đổi trạng thái `ms`.
- `loop()`: ở trạng thái `BLINK`, so sánh `millis()` để đổi mức chân LED, không dùng `delay()`.

## 5. Build và nạp

Cần VS Code + PlatformIO IDE (hoặc PlatformIO Core).

1. Mở thư mục dự án bằng VS Code.
2. Chọn env đúng với board (thanh dưới cùng, hoặc `pio run -e esp32c3_zero`).
3. Build, sau đó Upload.
   ```
   pio run -e esp32c3_zero -t upload
   ```
4. Nếu board không nhận lần đầu: giữ nút BOOT, bấm RESET, rồi nạp lại.

## 6. Cách thử

1. Bấm nút BOOT một lần: LED bật. Bấm thêm một lần: LED tắt.
2. Bấm nhanh hai lần: LED nháy liên tục, mỗi 200 ms.
3. Khi đang nháy, bấm một lần: LED tắt.

## 7. Ghi chú

- Dự án được viết lại từ ví dụ của thầy Nguyễn Anh Tuấn, thay thao tác "giữ nút" bằng "double click".
- Thư viện OneButton: https://github.com/mathertel/OneButton
