package com.example.testfreetype.adapter;

import android.content.Context;
import android.graphics.Typeface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.testfreetype.R;
import com.example.testfreetype.bean.FontItem;
import com.example.testfreetype.util.PathHelp;
import com.example.testfreetype.util.StorageHelper;

import java.io.File;
import java.util.List;


public class TextFontAdapter extends RecyclerView.Adapter<TextFontAdapter.ViewHolder> {

    private static final String TAG = "TextFontAdapter";

    private Context mContext;

    private List<FontItem> mFontList;

    private OnFontSelectListener mListener;

    public TextFontAdapter(Context context, List<FontItem> fontItemList, OnFontSelectListener listener) {
        mContext = context;
        mFontList = fontItemList;
        mListener = listener;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View root = LayoutInflater.from(mContext).inflate(R.layout.item_text_font, parent, false);

        return new ViewHolder(root);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        FontItem fontItem = mFontList.get(position);
        Typeface typeface = Typeface.createFromFile(PathHelp.INSTANCE.getFontsPath(mContext)+File.separator+fontItem.getFontPath());
        holder.fontShow.setTypeface(typeface);
//        if (fontItem.isChinese) {
//
//        } else {
//            holder.fontShow.setText("abc");
//        }
        holder.fontShow.setText("汉");
        holder.fontName.setText(fontItem.getFontName());

        if (fontItem.getSelected()) {
            holder.fontShow.setTextColor(mContext.getResources().getColor(R.color.font_red));
            holder.fontShow.setBackgroundResource(R.drawable.bg_red_round_corner);
            holder.fontName.setTextColor(mContext.getResources().getColor(R.color.font_red));
        } else {
            holder.fontShow.setTextColor(mContext.getResources().getColor(R.color.colorWhite));
            holder.fontShow.setBackgroundResource(R.drawable.bg_white_corner);
            holder.fontName.setTextColor(mContext.getResources().getColor(R.color.colorWhite));
        }
    }

    @Override
    public int getItemCount() {
        return mFontList.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        TextView fontShow;
        TextView fontName;

        public ViewHolder(View itemView) {
            super(itemView);

            fontShow = itemView.findViewById(R.id.tv_font_show);
            fontName = itemView.findViewById(R.id.tv_font_name);

            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            int index = getAdapterPosition();
            if (index >= 0) {
                for (FontItem fontItem : mFontList) {
                    fontItem.setSelected(false);
                }
                mFontList.get(index).setSelected(true);
                notifyDataSetChanged();
                if (mListener != null) {
                    mListener.fontSelected(index);
                }
            }
        }
    }


    public interface OnFontSelectListener {
        void fontSelected(int position);
    }
}
