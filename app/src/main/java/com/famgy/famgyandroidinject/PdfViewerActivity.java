package com.famgy.famgyandroidinject;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.widget.Toast;

import com.github.barteksc.pdfviewer.PDFView;

import java.io.File;
import java.util.ArrayList;

public class PdfViewerActivity extends AppCompatActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pdf_viewer);

        PDFView pdfView=(PDFView)findViewById(R.id.pdf_view);
        String resourcePath = Environment.getExternalStorageDirectory() + "/Download";
        File file = new File(resourcePath, "pdf_test.pdf");
        try {
            // 加载文件
            if (file.exists()) {
                pdfView.fromFile(file)
                        // .defaultPage(1)  //显示页数
                        .swipeHorizontal(false)  //fales 上下翻页  true 左右
                        .enableAntialiasing(true)  //是否页面渲染
                        .enableSwipe(true) //
                        //.pages(1,2)   //可以过滤的页数
                        .load();
            } else {
                pdfView.fromAsset("pdf_test.pdf").load();
            }
        } catch (Exception ex) {
            Toast.makeText(PdfViewerActivity.this,"文件不存在或已损坏",Toast.LENGTH_SHORT).show();
            finish();
        }
    }
}
