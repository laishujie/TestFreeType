package com.example.testfreetype.adapter;

import android.content.Context;
import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.testfreetype.R;
import com.example.testfreetype.bean.ImgItem;
import com.example.testfreetype.util.PathHelp;

import java.io.File;
import java.util.List;


public class TmpAdapter extends RecyclerView.Adapter<TmpAdapter.ViewHolder> {

    private static final String TAG = "TextFontAdapter";

    private final Context mContext;

    private final List<ImgItem> mFontList;

    private final OnImgSelectListener mListener;

    private final String imgPath;

    public TmpAdapter(Context context, List<ImgItem> fontItemList, OnImgSelectListener listener) {
        mContext = context;
        mFontList = fontItemList;
        mListener = listener;
        imgPath = PathHelp.INSTANCE.getTemplatePath(context);
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View root = LayoutInflater.from(mContext).inflate(R.layout.item_tmp, parent, false);

        return new ViewHolder(root);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        ImgItem fontItem = mFontList.get(position);
        holder.ivImg.setImageBitmap(BitmapFactory.decodeFile(imgPath + File.separator + fontItem.getImagePath()));

        if (fontItem.getSelected()) {
            holder.ivImg.setBackgroundResource(R.drawable.bg_red_round_corner);
        } else {
            holder.ivImg.setBackgroundResource(R.drawable.bg_white_corner);
        }
    }

    @Override
    public int getItemCount() {
        return mFontList.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        ImageView ivImg;

        public ViewHolder(View itemView) {
            super(itemView);

            ivImg = itemView.findViewById(R.id.iv_show);
            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            int index = getAdapterPosition();
            if (index >= 0) {
                for (ImgItem fontItem : mFontList) {
                    fontItem.setSelected(false);
                }
                mFontList.get(index).setSelected(true);
                notifyDataSetChanged();
                if (mListener != null) {
                    mListener.selected(index);
                }
            }
        }
    }


    public interface OnImgSelectListener {
        void selected(int position);
    }
}
