#pragma once
namespace sf {
#define THROW_IF_ERR2(v) THROW_IF_ERR((v == TRUE))
  struct list_view
  {
    list_view(HWND hwnd = NULL);
    void attach(HWND hwnd) {hwnd_ = hwnd;}
    virtual ~list_view();
    rect approximate_view_rect(int cx,int cy,int icount)
    {
      rect r;
      DWORD ret = ListView_ApproximateViewRect(hwnd_,cx,cy,icount);
      r.width = LOWORD(ret);
      r.height = HIWORD(ret);
    }
    bool arrange(uint32 code)
    {
      return ListView_Arrange(hwnd_,code);
    }
    void cancel_edit_label()
    {
      ListView_CancelEditLabel(hwnd_);
    }

    HIMAGELIST create_drag_image(int iItem,POINT& p)
    {
      return ListView_CreateDragImage(hwnd_,iItem,&p);
    }

    bool delete_all_items()
    {
      return ListView_DeleteAllItems(hwnd_) == TRUE;
    }

    bool delete_column(int column)
    {
      return ListView_DeleteColumn(hwnd_,column) == TRUE;
    }

    bool delete_item(int item)
    {
      return ListView_DeleteItem(hwnd_,item) == TRUE;
    }

    HWND edit_label(int item)
    {
      return ListView_EditLabel(hwnd_,item);
    }

    int enable_group_view(bool enable)
    {
      return ListView_EnableGroupView(hwnd_,enable?TRUE:FALSE);
    }

    bool ensure_visible(int index,bool partial_ok)
    {
      return ListView_EnsureVisible(hwnd_,index,partial_ok?TRUE:FALSE) == TRUE;
    }

    int find_item(int start,LVFINDINFO& info)
    {
      return ListView_FindItem(hwnd_,start,&info);
    }

    COLORREF get_background_color()
    {
      return ListView_GetBkColor(hwnd_);
    }

    void get_background_image(LVBKIMAGEW& image)
    {
      THROW_IF_ERR2(ListView_GetBkImage(hwnd_,&image));
    }

    uint32 get_call_back_mask()
    {
      return ListView_GetCallbackMask(hwnd_);
    }

    bool get_check_state(uint32 index){
      return ListView_GetCheckState(hwnd_,index) == TRUE;
    }

    void get_column(int column,LVCOLUMN& info)
    {
      if(ListView_GetColumn(hwnd_,column,&info) != TRUE)
      {
        throw win32_error_exception();
      }
    }

    template <typename Array>
    void get_column_order_array(int count,Array& arr)
    {
      THROW_IF_ERR2(ListView_GetColumnOrderArray(count,&arr[0]));
    }

    int get_column_width(int column)
    {
      return ListView_GetColumnWidth(hwnd_,column);
    }

    int get_count_per_page()
    {
      return ListView_GetCountPerPage(hwnd_);
    }

    HWND get_edit_control()
    {
      return ListView_GetEditControl(hwnd_);
    }

    void get_empty_text(std::wstring& text,uint32 size)
    {
      std::unique_ptr<wchar_t[]> temp(new wchar_t[size]);
      THROW_IF_ERR2(ListView_GetEmptyText(hwnd_,temp.get(),size));
      text = temp.get();
    }

    DWORD get_extended_style()
    {
      return ListView_GetExtendedListViewStyle(hwnd_);
    }

    int get_focused_group()
    {
      return ListView_GetFocusedGroup(hwnd_);
    }

    void get_footer_info(LVFOOTERINFO& info)
    {
      THROW_IF_ERR2(ListView_GetFooterInfo(hwnd_,&info));
    }

    void get_footer_item(uint32 index,LVFOOTERITEM& footer_item)
    {
      THROW_IF_ERR2(ListView_GetFooterItem(hwnd_,index,&footer_item));
    }

    void get_footer_item_rect(uint32 index,RECT& rect)
    {
      THROW_IF_ERR2(ListView_GetFooterItemRect(hwnd_,index,&rect));
    }

    void get_footer_rect(RECT& rect){
      THROW_IF_ERR2(ListView_GetFooterRect(hwnd_,&rect));
    }

    int get_group_count(){
      return ListView_GetGroupCount(hwnd_);
    }

    HIMAGELIST get_group_header_image_list()
    {
      return ListView_GetGroupHeaderImageList(hwnd_);
    }

    int get_group_info(int id,LVGROUP& group)
    {
      return ListView_GetGroupInfo(hwnd_,id,&group);
    }

    void get_group_info_by_index(int index, LVGROUP& group)
    {
      THROW_IF_ERR2(ListView_GetGroupInfoByIndex(hwnd_,index,&group));
    }

    void get_group_metrics( LVGROUPMETRICS& metrics){
      ListView_GetGroupMetrics(hwnd_,&metrics);
    }

    void get_group_rect(int id,uint32 type,RECT& rect)
    {
      THROW_IF_ERR2(ListView_GetGroupRect(hwnd_,id,type,&rect));
    }

    uint32 get_group_state(uint32 id,uint32 mask)
    {
      return ListView_GetGroupState(hwnd_,id,mask);

    }

    HWND get_header() {
      return ListView_GetHeader(hwnd_);
    }

    HCURSOR get_hot_cursor()
    {
      return ListView_GetHotCursor(hwnd_);
    }

    int get_hot_item()
    {
      return ListView_GetHotItem(hwnd_);
    }

    uint32 get_hover_time(){
      return ListView_GetHoverTime(hwnd_);
    }

    HIMAGELIST get_image_list(int param)
    {
      return ListView_GetImageList(hwnd_,param);
    }

    void get_insert_mark(LVINSERTMARK& mark)
    {
      THROW_IF_ERR2(ListView_GetInsertMark(hwnd_,&mark));
    }

    COLORREF get_insert_mark_color()
    {
      return ListView_GetInsertMarkColor(hwnd_);
    }

    int get_insert_mark_rect(RECT& rect){
      return ListView_GetInsertMarkRect(hwnd_,&rect);
    }

    int get_index_search_string(std::wstring& str){
      return ListView_GetISearchString(hwnd_,str.c_str());
    }

    void get_item(LVITEM& item)
    {
      THROW_IF_ERR2(ListView_GetItem(hwnd_,&item));
    }

    int get_item_count()
    {
      return ListView_GetItemCount(hwnd_);
    }

    void get_item_index_rect(LVITEMINDEX& index,uint32 subitem,uint32 code,RECT& rect)
    {
      THROW_IF_ERR2(ListView_GetItemIndexRect(hwnd_,&index,subitem,code,&rect));

    }

    void get_item_position(int index,POINT& point)
    {
      THROW_IF_ERR2(ListView_GetItemPosition(hwnd_,index,&point));
    }

    void get_item_rect(int index,RECT& rect,int code){
      THROW_IF_ERR2(ListView_GetItemRect(hwnd_,index,&rect,code));
    }

    bool get_item_spacing(bool small_)
    {
      return ListView_GetItemSpacing(hwnd_,small_?TRUE:FALSE) == TRUE;
    }

    uint32 get_item_state(int index,uint32 mask)
    {
      return ListView_GetItemState(hwnd_,index,mask);
    }

    std::wstring get_item_text(int index,int subitem_index)
    {
      wchar_t str[2048];
      ListView_GetItemText(hwnd_,index,subitem_index,str,2048);
      return std::wstring(str);
    }

    int get_next_item(int start_index,uint32 flags)
    {
      return ListView_GetNextItem(
        hwnd_,
        start_index,
        flags
        );

    }

    void get_next_item_index(LVITEMINDEX& itemindex,uint32 flags)
    {
      THROW_IF_ERR2(ListView_GetNextItemIndex(hwnd_,&itemindex,flags));
    }

    uint32 get_number_of_work_areas()
    {
      uint32 v;
      ListView_GetNumberOfWorkAreas(hwnd_,&v);
      return v;
    }

    POINT get_origin()
    {
      POINT o;
      THROW_IF_ERR2(ListView_GetOrigin(hwnd_,&o));
      return o;
    }

    COLORREF get_outline_color()
    {
      return ListView_GetOutlineColor(hwnd_);
    }

    uint32 get_selected_column()
    {
      return ListView_GetSelectedColumn(hwnd_);
    }

    uint32 get_selected_count()
    {
      return ListView_GetSelectedCount(hwnd_);
    }

    int get_selection_mark(){
      return ListView_GetSelectionMark(hwnd_);
    }

    int get_string_width(const std::wstring& str){
      return ListView_GetStringWidth(hwnd_,str.c_str());
    }

    void get_subitem_rect(int item_index,int sub_item_index,int code,RECT& rect)
    {
      THROW_IF_ERR2(ListView_GetSubItemRect(hwnd_,item_index,sub_item_index,code,&rect));
    }

    COLORREF get_text_bkcolor(){
      return ListView_GetTextBkColor(hwnd_);
    }

    COLORREF get_text_tcolor(){
      return ListView_GetTextColor(hwnd_);
    }


    void get_tile_info( LVTILEINFO& info ){
      THROW_IF_ERR2(ListView_GetTileInfo(hwnd_,&info));
    }

    void get_tile_view_info(LVTILEVIEWINFO& info){
      THROW_IF_ERR2(ListView_GetTileViewInfo(hwnd_,&info));
    }

    HWND get_tooltips(){
      return ListView_GetToolTips(hwnd_);
    }

    int get_top_index(){
      return ListView_GetTopIndex(hwnd_);
    }

    bool get_unicode_format(){
      return ListView_GetUnicodeFormat(hwnd_) == TRUE;
    }

    uint32 get_view(){
      return ListView_GetView(hwnd_);
    }

    void get_view_rect(RECT& rect){
      THROW_IF_ERR2(ListView_GetViewRect(hwnd_,&rect));
    }

    void get_workareas(int work_areas,RECT& rect){
      ListView_GetWorkAreas(hwnd_,work_areas,&rect);
    }

    bool has_group(int group_id){
      return ListView_HasGroup(hwnd_,group_id);
    }


    int hit_test(LVHITTESTINFO& info){
      return ListView_HitTest(hwnd_,&info);
    }

    int hit_test_ex(LVHITTESTINFO& info){
      return ListView_HitTestEx(hwnd_,&info);
    }

    int insert_column(int column_index,LVCOLUMN& column){
      return ListView_InsertColumn(hwnd_,column_index,&column);
    }

    int insert_group(int index,LVGROUP& group){
      return ListView_InsertGroup(hwnd_,index,&group);
    }

    void insert_group_sorted( LVINSERTGROUPSORTED& group_sorted ){
      ListView_InsertGroupSorted(hwnd_,&group_sorted);
    }

    int insert_item(LVITEM& item){
      return ListView_InsertItem(hwnd_,&item);
    }

    bool insert_mark_hit_test(POINT& point,LVINSERTMARK mark){
      return ListView_InsertMarkHitTest(hwnd_,&point,&mark) == TRUE;
    }

    bool is_group_view_enabled(){
      return ListView_IsGroupViewEnabled(hwnd_) == TRUE;
    }

    uint32 is_item_visible(uint32 index){
      return ListView_IsItemVisible(hwnd_,index);
    }

    uint32 map_id_to_index(uint32 id){
      return ListView_MapIDToIndex(hwnd_,id);
    }

    uint32 map_index_to_id(uint32 index){
      return ListView_MapIndexToID(hwnd_,index);
    }

    void redraw_items(int first,int last){
      THROW_IF_ERR2(ListView_RedrawItems(hwnd_,first,last));
    }

    void remove_all_groups(){
      ListView_RemoveAllGroups(hwnd_);
    }

    int remove_group(int group_id){
      return ListView_RemoveGroup(hwnd_,group_id);
    }

    void scroll(int dx,int dy){
      THROW_IF_ERR2(ListView_Scroll(hwnd_,dx,dy));
    }

    list_view& set_background_color(COLORREF color){
      THROW_IF_ERR2(ListView_SetBkColor(hwnd_,color));
      return *this;
    }

    list_view& set_background_image( LVBKIMAGE& image){
      ListView_SetBkImage(hwnd_,&image);
      return *this;
    }

    list_view& set_callback_mask(uint32 mask){
      THROW_IF_ERR2(ListView_SetCallbackMask(hwnd_,mask));
      return *this;
    }

    list_view& set_check_state(uint32 index,bool check){
      ListView_SetCheckState(hwnd_,index,check);
      return *this;
    }

    list_view& set_column(int column_index,const LVCOLUMN& column)
    {
      THROW_IF_ERR2(ListView_SetColumn(hwnd_,column_index,&column));
      return *this;
    }
    template <typename Array>
    list_view& set_column_order_array(Array& arr){
      THROW_IF_ERR2(ListView_SetColumnOrderArray(hwnd_,arr.size(),&arr[0]));
      return *this;
    }

    list_view& set_column_width(int column,int cx){
      THROW_IF_ERR2(ListView_SetColumnWidth(hwnd_,column,cx));
      return *this;
    }

    list_view& set_extended_list_view_style(uint32 style)
    {
      ListView_SetExtendedListViewStyle(hwnd_,style);
      return *this;
    }

    list_view& set_extended_list_view_style_ex(uint32 mask,uint32 style){
      ListView_SetExtendedListViewStyleEx(hwnd_,mask,style);
      return *this;
    }

    list_view& set_group_header_image_list(HIMAGELIST iml,HIMAGELIST& previous){
      previous = ListView_SetGroupHeaderImageList(hwnd_,iml);
      return *this;
    }

    list_view& set_group_info(int group_id,LVGROUP& group,int& index){
      index = ListView_SetGroupInfo(hwnd_,group_id,&group);
      return *this;
    }

    list_view& set_group_metrics(LVGROUPMETRICS &metrics){
      ListView_SetGroupMetrics(hwnd_,&metrics);
      return *this;
    }

    list_view& set_group_state(uint32 group_id,uint32 mask,uint32 state){
      ListView_SetGroupState(hwnd_,group_id,mask,state);
      return *this;
    }

    list_view& set_hot_cursor(HCURSOR cursor,HCURSOR& previous)
    {
      previous = ListView_SetHotCursor(hwnd_,cursor);
      return *this;
    }

    list_view& set_hot_item(int index,int& previous){
      previous = ListView_SetHotItem(hwnd_,index);
      return *this;
    }

    list_view& set_hover_time(uint32 hover_time){
      ListView_SetHoverTime(hwnd_,hover_time);
      return *this;
    }

    list_view& set_icon_spacing(int cx,int cy,int& previous_cx,int& previous_cy )
    {
      DWORD ret = ListView_SetIconSpacing(hwnd_,cx,cy);
      previous_cx = LOWORD(ret);
      previous_cy = HIWORD(ret);
      return *this;
    }

    list_view& set_image_list(HIMAGELIST himl,int type,HIMAGELIST& previous){
      previous = ListView_SetImageList(hwnd_,himl,type);
      return *this;
    }

    list_view& set_info_tip(LVSETINFOTIP& tip){
      THROW_IF_ERR2(ListView_SetInfoTip(hwnd_,&tip));
      return *this;
    }

    list_view& set_insert_mark(LVINSERTMARK& mark){
      THROW_IF_ERR2(ListView_SetInsertMark(hwnd_,&mark));
      return *this;
    }

    list_view& set_insert_mark_color(COLORREF color,COLORREF& previous){
      previous = ListView_SetInsertMarkColor(hwnd_,color);
      return *this;
    }

    list_view& set_item(LVITEM &item){
      THROW_IF_ERR2(ListView_SetItem(hwnd_,&item));
      return *this;
    }

    list_view& set_item_count(int count){
      ListView_SetItemCount(hwnd_,count);
      return *this;
    }

    list_view& set_item_count_ex(int index,uint32 flags){
      ListView_SetItemCountEx(hwnd_,index,flags);
      return *this;
    }

    list_view& set_item_index_state(LVITEMINDEX& index,int data,int mask)
    {
      ListView_SetItemIndexState(hwnd_,&index,data,mask);
      return *this;
    }

    list_view& set_item_position(int index,int x,int y){
      THROW_IF_ERR2(ListView_SetItemPosition(hwnd_,index,x,y));
      return *this;
    }

    list_view& set_item_position32(int index,int x,int y){
      ListView_SetItemPosition32(hwnd_,index,x,y);
      return *this;
    }

    list_view& set_item_state(int index,uint32 state,uint32 mask){
      ListView_SetItemState(hwnd_,index,state,mask);
      return *this;
    }

    list_view& set_item_text(int index,int sub_item_index,const std::wstring text)
    {
      ListView_SetItemText(hwnd_,index,sub_item_index,const_cast<wchar_t*>(text.c_str()));
      return *this;
    }

    list_view& set_outline_color(COLORREF color,COLORREF& previous){
      previous = ListView_SetOutlineColor(hwnd_,color);
      return *this;
    }

    list_view& set_selected_column(int column){
      ListView_SetSelectedColumn(hwnd_,column);
      return *this;
    }

    list_view& set_selection_mark(int index){
      ListView_SetSelectionMark(hwnd_,index);
      return *this;
    }


    list_view& set_text_background_color(COLORREF color){
      THROW_IF_ERR2(ListView_SetTextBkColor(hwnd_,color));
      return *this;
    }

    list_view& set_text_color(COLORREF color){
      THROW_IF_ERR2(ListView_SetTextColor(hwnd_,color));
      return *this;
    }

    list_view& set_tile_info(LVTILEINFO& info){
      THROW_IF_ERR2(ListView_SetTileInfo(hwnd_,&info));
      return *this;
    }

    list_view& set_tile_view_info(LVTILEVIEWINFO &info){
      THROW_IF_ERR2(ListView_SetTileViewInfo(hwnd_,&info));
      return *this;
    }

    list_view& set_tooltips(HWND tooltip,HWND& previous)
    {
      previous = ListView_SetToolTips(hwnd_,tooltip);
      return *this;
    }

    list_view& set_unicode_format(bool unicode,bool& previous){
      previous = ListView_SetUnicodeFormat(hwnd_,unicode);
      return *this;
    }

    list_view& set_view(uint32 param){
      ListView_SetView(hwnd_,param);
      return *this;
    }

    list_view& set_work_areas(int work_areas,RECT& rect)
    {
      ListView_SetWorkAreas(hwnd_,work_areas,&rect);
      return *this;
    }

    void sort_groups(PFNLVGROUPCOMPARE compare,LPVOID p){
      THROW_IF_ERR2(ListView_SortGroups(hwnd_,compare,p));
    }

    void sort_items(PFNLVCOMPARE compare,uint32 param){
      THROW_IF_ERR2(ListView_SortItems(hwnd_,compare,param));
    }

    void sort_items_ex(PFNLVCOMPARE compare,uint32 param){
      THROW_IF_ERR2(ListView_SortItemsEx(hwnd_,compare,param));
    }

    int sub_item_hit_test(LVHITTESTINFO &info){
      return ListView_SubItemHitTest(hwnd_,&info);
    }


    int sub_item_hit_test_ex(LVHITTESTINFO& info){
      return ListView_SubItemHitTestEx(hwnd_,&info);
    }

    void update(int index){
      THROW_IF_ERR2(ListView_Update(hwnd_,index));
    }

  private:
    HWND hwnd_;
  };
}

