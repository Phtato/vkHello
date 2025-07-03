import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

/*class MyXComponentController extends XComponentController {
  onSurfaceCreated(surfaceId: string): void {
    console.log(`onSurfaceCreated surfaceId: ${surfaceId}`)
    //testNapi.SetSurfaceId(BigInt(surfaceId));
  }

*//*  onSurfaceChanged(surfaceId: string, rect: SurfaceRect): void {
    console.log(`onSurfaceChanged surfaceId: ${surfaceId}, rect: ${JSON.stringify(rect)}}`)
    // 在onSurfaceChanged中调用ChangeSurface绘制内容
    testNapi.ChangeSurface(BigInt(surfaceId), rect.surfaceWidth, rect.surfaceHeight)
  }

  onSurfaceDestroyed(surfaceId: string): void {
    console.log(`onSurfaceDestroyed surfaceId: ${surfaceId}`)
    testNapi.DestroySurface(BigInt(surfaceId))
  }*//*
}*/

@Entry
@Component
struct Index {


  build() {
    Row() {
      Column() {
        XComponent({ id: 'xcomponentId', type: XComponentType.SURFACE, libraryname: 'entry'})
          .width(360)
          .height(360)
        Button(){
          Text("加载")
            .fontSize(20)
            .fontColor(Color.White)
        }
        .width(180)
        .height(50)
        .margin({
          top:20
        })
        .onClick(()=>{
          testNapi.run();
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}
