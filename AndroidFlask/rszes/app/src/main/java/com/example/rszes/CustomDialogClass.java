package com.example.rszes;

import android.app.Activity;
import android.app.Dialog;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;

import java.io.InputStream;

public class CustomDialogClass extends Dialog implements
        android.view.View.OnClickListener {

    public Activity c;
    public Dialog d;
    public Button ok;
    public ImageView imageView;
    public Bitmap bitmapImage;

    public CustomDialogClass(Activity a, Bitmap bmp) {
        super(a);
        // TODO Auto-generated constructor stub
        bitmapImage = bmp;
        this.c = a;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.custom_dialog);
        ok = (Button) findViewById(R.id.btn_ok);
        ok.setOnClickListener(this);
        imageView = (ImageView) findViewById(R.id.imageView);
        imageView.setImageBitmap(bitmapImage);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_ok:
                c.finish();
                break;
            default:
                break;
        }
        dismiss();
    }
}
