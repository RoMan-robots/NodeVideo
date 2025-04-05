// @ts-ignore
import screenCapture from './addons/screen_capture.node';

(screenCapture as any).startStream((frame: any) => {
    console.log('Frame:', frame);
});

setTimeout(() => {
    (screenCapture as any).stopStream();
}, 5000);
