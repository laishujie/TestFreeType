package com.example.testfreetype

import android.Manifest
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityMainBinding
import com.example.testfreetype.util.PathHelp
import com.example.testfreetype.util.StorageHelper
import java.io.File

class MainActivity : AppCompatActivity() {

    private val sdfPath by lazy {
        StorageHelper.getInternalFilesDir(this).absolutePath + File.separator + "sdf_test.png"
    }

    private val ttfPath by lazy {
        StorageHelper.getInternalFilesDir(this).absolutePath + File.separator + "fonts/DroidSansFallback.ttf"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val binding = ActivityMainBinding.inflate(layoutInflater)

        setContentView(binding.root)


        binding.btnCreateSdf.setOnClickListener {
            val intent = Intent(this, SdfShaderActivity::class.java)
            intent.putExtra("path", sdfPath)
            startActivity(intent)
        }


        binding.btnInset.setOnClickListener {
            val intent = Intent(this, TextureManagerActivity::class.java)
            intent.putExtra("path", ttfPath)
            startActivity(intent)
        }

        binding.btnEdit.setOnClickListener {
            val intent = Intent(this, TextShaderActivity::class.java)
            intent.putExtra("path", ttfPath)
            startActivity(intent)
        }
        binding.btnTextEdit.setOnClickListener {
            val intent = Intent(this, TextEditActivity::class.java)
            intent.putExtra("path", ttfPath)
            startActivity(intent)
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(
                arrayOf(
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
                ), 0
            )
        }

        val cameraPath = StorageHelper.getCameraPath(this)
            Log.e("11111","cameraPath $cameraPath")
        Thread(Runnable {
            StorageHelper.copyAssetToPath(this, "sdf_test.png", sdfPath)
            StorageHelper.copyFilesFromAssets(this, "template", PathHelp.getTemplatePath(this))
            StorageHelper.copyFilesFromAssets(this, "fonts", PathHelp.getFontsPath(this))
            //StorageHelper.copyAssetToPath(this, "DroidSansFallback.ttf", ttfPath)
        }).start()

    }
}