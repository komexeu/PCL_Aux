# PCL_Aux  
***version:**  tmp_version 0.5a*
***平台 :** Window 10 x64*

## 簡介 :
> 尚未完成之EXE檔，僅供臨時分割使用。

## 安裝說明:
> 將Aux_release1.7z解壓縮

> 將下方IntelRealsense.dll / realsense2.dll / PCLAuxilary.exe覆蓋進Aux_release1.7z解壓縮後的檔案。

> 點選PCLAuxilary.exe即可直接使用。

## 使用方法:
### Data Import(點雲輸入):
* 可輸入.pcd與.csv
> 使用限制 :
> - pcd格式為PointXYZRGB(x,y,z,rgb)
> - csv格式為(x,y,z,r,g,b)
> - 點雲超過400,000點時載入速度較慢

### Data Export(點雲輸出):
* 輸出.pcd
* 點選一個圖層，自動抓取底下已分割完成的所有子類圖層資訊![enter image description here](https://lh3.googleusercontent.com/KUdm1MbcOi-JJCebyhAIEnhPgmYNxvcMX77bDtMtpFV3U1S5C6ajBwsBvQhpotaCYewfRrISa6Sd "被輸出的圖層")
![enter image description here](https://lh3.googleusercontent.com/u5WG7KUy75NZNyNzja8xSYdKZT5rDHmyKULaDM8s8XGh-SgRabUgkdEePjVJpKvi-vUoHuw9JvT5)
> 使用限制 :
> - 僅輸出.pcd(x,y,z,rgb,ID)

### realsense point pick(取像模式):
> 使用限制 :
>- 暫時鎖定，無法使用
 
### Slider Segmentate(滑條分割模式):
 * 拉動滑條進行快速分割預覽
![enter image description here](https://lh3.googleusercontent.com/np_p8MDki5VO9qeWQfCEd5T53AiP7YxEBtIiiFy1-ZfcBV7mi7qPneEcgeM2r3ic_SJUAT0yrbxu)
![enter image description here](https://lh3.googleusercontent.com/UiHMsLlKAWvuGrkUwWH-JQ3--WBxF-KS-0-_hzjB_bzk0QNaA3sbOqs9HjGdn7THoeIcWzX1gIdG)
* 點選Slider Segmentate確定分割。
> 使用限制 :
>- 因效能問題暫時將可處理資料點設定在100,000內。
>- 大於100,000將無法進行分割，請使用手動分割。

### User Segmentate(已選點手動分割):
* 對手動分割工具(X框選/B筆刷)進行標記。
![enter image description here](https://lh3.googleusercontent.com/Vp-mIDXIQh4BhgluwiLmi9FpfDQeU1sLrA0oyziWwTOG4ZXQjwbdm0YuRjcug0ce7TRVLV9d0KOQ)
![enter image description here](https://lh3.googleusercontent.com/wJlzmhfbq5jVY2sy_g7Jj5iSDXzwpqKqpzndwH5I9pgMiGvgBesR5b1Ncl3Mcvd1_y_DrqKCZzsf)
* 點選User Segmentate確定分割。

### smooth(平滑化):
* 對不平滑點雲進行平滑化
> 使用限制 :
>- 資料點太大或電腦效能較不佳處理時序較久，有可能造成崩潰。
 
 ## 工具快捷鍵
|快捷鍵|功能| 備註 | 
|-----|----|-----|
| X |手動標記:框選(建議使用選項) |按下"X"來啟動/取消使用工具|
|B|手動標記:筆刷(資料點太多時效能較差)|按下"B"來啟動/取消使用工具。|
|A|視角轉換|六方位視角變換|
|Ctrl|手動標記中使用，加選|
|Alt|手動標記中使用，減選|
|N|筆刷縮小|
|M|筆刷放大|太大時效能較差，有可能當機

