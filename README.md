# Obstacle_Dection_System
Obstacle Detection System là một hệ thống nhúng được phát triển nhằm phát hiện và đo khoảng cách tới vật cản trong môi trường thực tế. Hệ thống sử dụng cảm biến siêu âm HC-SR04, module UART để truyền nhận dữ liệu, và màn hình OLED để hiển thị thông tin khoảng cách. Ngoài ra, một nhân hệ điều hành thời gian thực (RTOS) tự phát triển được tích hợp để quản lý đa nhiệm.
### Thành phần chính:
- us100/: Driver giao tiếp với cảm biến khoảng cách HC-SR04.
- UART/: Thư viện UART tùy chỉnh để truyền nhận dữ liệu nối tiếp.
- OLED/: Giao tiếp và hiển thị thông tin trên màn hình OLED 0.96 inch.
- osKernel/: RTOS tự phát triển tối giản, hỗ trợ quản lý task cơ bản.
- main.c: Chương trình chính khởi tạo hệ thống và xử lý luồng công việc.
- UI/: Giao diện hiển thị của hệ thống sử dụng framework Qt.
### Chức năng
- Đo khoảng cách đến vật cản bằng các cảm biến HC-SR04.
- Hiển thị kết quả đo trên màn hình OLED và gửi dữ liệu lên PC thông qua UART để hiển thị lên UI.
- Quản lý nhiều luồng đo đạc và cập nhật hiển thị bằng hệ điều hành tự xây dựng.
### Yêu cầu phần cứng
- Vi điều khiển STM32
- Cảm biến siêu âm HC-SR04
- Màn hình OLED 0.96 inch (giao tiếp I2C)
- Bộ chuyển USB-UART\
**Video demo: https://youtu.be/1uV4va07ZqE**
