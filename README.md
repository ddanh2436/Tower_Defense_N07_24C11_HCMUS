🚩 TOWER DEFENSE_N07_24C11_HCMUS


Đây là một dự án game Tower Defense cổ điển được xây dựng bằng ngôn ngữ C++ và thư viện đồ họa SFML (Simple and Fast Multimedia Library). Người chơi sẽ phải sử dụng tư duy chiến thuật để xây dựng các tháp phòng thủ, ngăn chặn các đợt tấn công của kẻ thù.

[![Menu chính của game](https://i.postimg.cc/W3cscQWn/Screenshot-2025-08-07-223557.png)](https://postimg.cc/4KPDzFT7)

✨ TÍNH NĂNG NỔI BẬT
   Lối chơi chiến thuật: Xây dựng và nâng cấp các loại tháp khác nhau để chống lại từng loại kẻ thù.

🗺️ Nhiều bản đồ: Game hỗ trợ nhiều bản đồ với các đường đi của quái vật khác nhau, đòi hỏi chiến thuật riêng cho từng màn.

🌊 Hệ thống wave: 8 đợt tấn công mỗi màn (10 đợt ở Mansion). Mỗi đợt là hỗn hợp nhiều loại quái, danh sách quái mở rộng dần theo đợt, và đợt cuối là boss Wizard kèm quân hộ tống.

💰 Kinh tế: Bắt đầu với 260 vàng. Dọn xong mỗi đợt được thưởng vàng, và gọi đợt tiếp theo sớm (phím N) được thưởng thêm theo số giây bỏ qua.

🛡️ Giáp (Armour): Mỗi loại quái có chỉ số giáp trừ thẳng vào **từng phát bắn**. Rider có giáp 9 nên mũi tên 18 sát thương chỉ còn 9, trong khi đạn pháo 46 vẫn vào 37. Đây là lý do bạn phải trộn cả hai loại trụ chứ không spam một loại.

🔮 Kỹ năng Rain of Fire: Lấy cảm hứng từ Kingdom Rush - nhấn **R** (hoặc nút góc dưới trái) rồi click lên bản đồ để gọi một đòn sát thương diện rộng **bỏ qua giáp**, hồi chiêu 25 giây. Rất hợp để cứu một đợt quái giáp dày đang sắp lọt.

👀 Xem trước đợt tới: Banner trên đỉnh màn hình liệt kê chính xác đợt sau gồm những loại quái nào, bao nhiêu con, giáp bao nhiêu - để bạn chọn trụ theo đúng thứ sắp tới.

💀 Boss lọt lưới mất nhiều mạng hơn: Rider/Rat mất 2 mạng, Wizard Boss mất 5 mạng.

🖥️ Tự động vừa mọi độ phân giải: Game được thiết kế trên khung 1920x1080 và tự letterbox vào màn hình thật, nên giao diện không bị lệch hay tràn ra ngoài trên máy có độ phân giải khác.

♞♜ Đa dạng kẻ thù và tháp:

Kẻ thù: Nhiều loại quái vật với các chỉ số máu, tốc độ, và giá trị tiền thưởng khác nhau (Goblin, Sói, Ong, Slime, v.v.).

Tháp: Hai loại tháp có vai trò khác hẳn nhau, mỗi loại 3 cấp nâng cấp:

  • Archer Tower (60 vàng): rẻ, tầm xa, bắn nhanh, sát thương đơn mục tiêu.

  • Cannon Tower (110 vàng): đắt, tầm gần, bắn chậm, gây sát thương lan (splash) cho cả nhóm quái đứng gần điểm nổ.

Đạn tự động bám theo mục tiêu, nên tháp không còn bắn hụt vào quái đang di chuyển. Tháp luôn ưu tiên bắn con quái đi xa nhất trên đường - con nguy hiểm nhất.

💾 Hệ thống lưu/tải game: Người chơi có thể lưu lại tiến trình của mình và tiếp tục chơi sau.

🥇 Bảng xếp hạng (Leaderboard): Ghi lại điểm số cao của người chơi, bao gồm tên, điểm, số quái đã diệt và thời gian hoàn thành.

🎵 Quản lý âm thanh: Tùy chỉnh âm lượng nhạc nền, hiệu ứng âm thanh, và có thể bật/tắt chúng trong phần cài đặt.

🎨 Đồ họa và hiệu ứng: Game sử dụng các sprite và animation để tạo ra trải nghiệm sống động.

🖥️ Giao diện người dùng (UI) đầy đủ: Bao gồm menu chính, màn hình chọn bản đồ, màn hình cài đặt, menu tạm dừng trong game và các thông báo khác.

🛠️ CÀI ĐẶT VÀ YÊU CẦU

Yêu cầu hệ thống:

🖥 Hệ điều hành: Windows.

Một trình biên dịch C++ hỗ trợ C++17 trở lên (ví dụ: Visual Studio 2019/2022).

📚 Thư viện cần thiết
SFML (>= 2.5.1): Thư viện đồ họa và đa phương tiện chính của dự án.

Bạn cần tải bản phù hợp với trình biên dịch của mình (ví dụ: Visual C++ 15 (2017) - 64-bit).

Hãy chắc chắn rằng bạn đã thiết lập đúng các đường dẫn Include Directories và Library Directories trong project, cũng như liên kết các file .lib cần thiết (sfml-graphics-d.lib, sfml-window-d.lib, sfml-system-d.lib, sfml-audio-d.lib, v.v. cho Debug và không có -d cho Release).

🌳📁 Cấu trúc thư mục
Để dự án chạy đúng, hãy đảm bảo các thư mục được sắp xếp như sau:

Tower/
|-- Tower.vcxproj
|-- MainPrg.cpp
|-- cgame.cpp
|-- cgame.h
|-- cmap.cpp
|-- cmap.h
|-- GameView.cpp
|-- GameView.h
|-- ... (các file .cpp và .h khác)
|
|-- assets/
|   |-- pixel_font.ttf
|   |-- menu_background.png
|   |-- menu_music.ogg
|   |-- game_music.ogg
|   |-- ... (tất cả các file hình ảnh và âm thanh khác)
|
|-- data/
|   |-- maps_index.txt
|   |-- map1.txt
|   |-- map2.txt
|   |-- map3.txt
|   |-- map4.txt
|   |-- savegame.txt
|   |-- leaderboards.txt
|
|-- include/
|   |-- SFML/
|       |-- (Toàn bộ các file header của SFML)
|
|-- lib/
|   |-- (Toàn bộ các file .lib của SFML)
|
|-- bin/
    |-- (Toàn bộ các file .dll của SFML)
Lưu ý quan trọng: File thực thi (.exe) sau khi biên dịch cần nằm cùng cấp với thư mục assets/ và data/ để game có thể tìm thấy tài nguyên.

🚀 CÁCH SỬ DỤNG

Biên dịch game: Mở dự án Tower.vcxproj trong Visual Studio, chọn cấu hình Debug hoặc Release, và biên dịch dự án. Đảm bảo rằng các thư viện SFML đã được liên kết đúng.

Chạy game: Chạy file Tower.exe đã được tạo ra.

Menu chính: Hiển thị các tuỳ chọn trong game.

New Game: Bắt đầu một màn chơi mới. Bạn sẽ được chuyển đến màn hình chọn bản đồ.

Load Game: Tải lại màn chơi đã được lưu từ file data/savegame.txt.

Leaderboard: Xem bảng xếp hạng điểm cao.

Settings: Tùy chỉnh âm thanh của game.

Exit: Thoát khỏi trò chơi.

⚔️⚔️ Trong game:

♜ Xây tháp: Thanh chọn trụ nằm ở góc dưới bên phải. Nhấn vào một loại trụ (di chuột lên trụ để xem bảng chỉ số: giá, sát thương, tầm bắn, tốc độ bắn, bán kính nổ), khi đó toàn bộ các ô có thể xây sẽ sáng lên màu xanh lá, kèm vòng tròn hiển thị tầm bắn tại vị trí con trỏ. Nhấn chuột trái vào một ô xanh để đặt tháp.

Người chơi có thể xây, nâng cấp và bán tháp bất cứ lúc nào - kể cả trong lúc đang có đợt tấn công và trong thời gian nghỉ giữa hai đợt.

⚒ Nâng cấp/Bán tháp: Nhấn chuột trái vào một tháp đã xây để hiện bảng điều khiển, vòng tròn tầm bắn và chỉ số của tháp. Nút UPGRADE sẽ bị làm mờ nếu bạn chưa đủ vàng.

▶️ Bắt đầu đợt tấn công: Nhấn phím N hoặc nhấn nút "START WAVE" ở giữa phía dưới màn hình. Gọi đợt sớm trong lúc nghỉ sẽ được thưởng vàng.

⏸ Tạm dừng: Nhấn Esc. Nếu đang chọn trụ hoặc đang mở bảng điều khiển tháp, Esc sẽ hủy thao tác đó trước; nhấn Esc lần nữa mới mở menu tạm dừng.

⏩ Tua nhanh: Nhấn phím Space hoặc nút tua nhanh ở góc trên bên phải để đổi tốc độ giữa 1x / 2x / 3x. Tốc độ hiện tại hiển thị ngay dưới nút.

🔥 Rain of Fire: Nhấn **R** hoặc nút ở góc dưới bên trái để kích hoạt, sau đó click vào vị trí muốn đánh. Vòng tròn đỏ theo con trỏ cho biết phạm vi. Nút hiển thị thời gian hồi chiêu còn lại.

🎯 Mục tiêu:

Ngăn chặn kẻ thù đi đến cuối con đường. Mỗi kẻ thù lọt qua sẽ làm bạn mất một mạng (bạn có 15 mạng). Bạn sẽ thua cuộc khi mất hết mạng. ❤️

Sống sót qua tất cả các đợt tấn công để chiến thắng màn chơi! 

![](https://i.postimg.cc/RF2KmVrY/Readme-Image.png)
