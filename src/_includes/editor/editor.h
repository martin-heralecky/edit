#ifndef _T_EDITOR_EDITOR_H
#define _T_EDITOR_EDITOR_H

#include "editor.h"

#include <functional>

#include <QWidget>
#include <QPixmap>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QCloseEvent>

#include <QTimer>

#include <QChar>
#include <QString>
#include <QStringList>

#include <QVector>
#include <QPair>

#include <QColor>
#include <QPoint>
#include <QSize>

#include "controls/label.h"

#include "editor/coordinates.h"
#include "editor/cursor.h"
#include "editor/drawing_manager.h"
#include "editor/vscrollbar.h"

#include "editor/find_dialog.h"
#include "editor/replace_dialog.h"


/**
 * The _t::editor::editor class representing one editor control
 * with specific text inside its area.
 */
class _t::editor::editor : public QWidget
{
    Q_OBJECT


    /**
     * The main QLabel-based element to which the canvas is associated.
     *
     * @var _t::controls::label area
     */
    controls::label area;

    /**
     * Text shift according to scrollbars.
     *
     * @var QPoint shift
     */
    QPoint shift = QPoint(0, 0);

    /**
     * The vertical scrollbar.
     *
     * @var _t::editor::vscrollbar * vscrollbar
     */
    _t::editor::vscrollbar *vscrollbar;


    /**
     * Tha canvas for painting the text.
     *
     * @var QPixmap canvas
     */
    QPixmap canvas;

    /**
     * The drawing manager that paints on the canvas.
     *
     * @var _t::editor::drawing_manager painter
     */
    _t::editor::drawing_manager painter;


    /**
     * @var QColor background
     */
    QColor background = QColor(60, 60, 70);

    /**
     * The background color for selected cells.
     *
     * @var QColor selection_background
     */
    QColor selection_background = QColor(80, 80, 90);

    /**
     * @var QSize cell_size
     */
    QSize cell_size;

    /**
     * @var QFont font
     */
    QFont font;

    /**
     * @var QColor font_color
     */
    QColor font_color = QColor(250, 250, 250);


    /**
     * @var _t::editor::cursor cursor
     */
    _t::editor::cursor cursor;

    /**
     * Timer for cursor blinking.
     *
     * @var QTimer cursor_timer
     */
    QTimer cursor_timer;

    /**
     * States if cursor, while blinking, is currently visible or not.
     *
     * @var bool cursor_visible
     */
    bool cursor_visible = false;


    /**
     * The actual text in the editor, saved in list for easy manipulation
     * with line lengths.
     *
     * @var QStringList text
     */
    QStringList text;


    /**
     * @var QChar newline_character
     */
    QChar newline_character = '\n';


    /**
     * @var const QVector<QPair<QColor, QString>> * sh_rules
     */
    const QVector<QPair<QColor, QString>> *sh_rules = 0;


    /**
     * @var _t::editor::find_dialog * find_d
     */
    _t::editor::find_dialog *find_d;

    /**
     * @var _t::editor::replace_dialog * replace_d
     */
    _t::editor::replace_dialog *replace_d;



    /**
     * Handles any user input from keyboard.
     *
     * @param QKeyEvent * event
     */
    void keyPressEvent(QKeyEvent *event);

    /**
     * Handles wheel event.
     *
     * @param QWheelEvent * event
     */
    void wheelEvent(QWheelEvent *event);

    /**
     * Handles focus-in event.
     *
     * @param QFocusEvent * event
     */
    void focusInEvent(QFocusEvent *event);

    /**
     * Handles focus-out event.
     *
     * @param QFocusEvent * event
     */
    void focusOutEvent(QFocusEvent *event);

    /**
     * Handles resize event.
     *
     * @param QResizeEvent * event
     */
    void resizeEvent(QResizeEvent *event);


    /**
     * Moves cursor one 'word' left.
     *
     * @param bool select
     */
    void go_word_left(bool select);

    /**
     * Moves cursor one 'word' right.
     *
     * @param bool select
     */
    void go_word_right(bool select);


    /**
     * Counts the cell size and loads it to the cell_size variable.
     */
    void update_cell_size();



    /**
     * Easy way to get line where the cursor is currently placed.
     *
     * @return QString &  Current line ref.
     */
    QString &active_line();

    /**
     * @param _t::editor::coordinates & from
     * @param _t::editor::coordinates & to
     */
    void get_selected_range(coordinates &from, coordinates &to) const;



    /**
     * Deselect all selected cells.
     */
    void deselect();

    /**
     * Redraws everything.
     */
    void redraw();


    /**
     * Performs the syntax highlighting.
     */
    void highlight_syntax();


    /**
     * Deletes the character at specified coordinates.
     *
     * @param const _t::editor::coordinates & coords
     */
    void delete_char(const coordinates &coords);

    /**
     * Deletes the characters in the specified range.
     * Ranges must be in ascending order.
     *
     * @param const _t::editor::coordinates & from
     * @param const _t::editor::coordinates & to
     */
    void delete_chars(const coordinates &from, const coordinates &to);


    /**
     * Apply selected-cell style to the cell.
     *
     * @param const _t::editor::coordinates & coords
     */
    void draw_selected_cell(const coordinates &coords);

    /**
     * Apply normal cell style to the cell.
     *
     * @param const _t::editor::coordinates & coords
     */
    void draw_deselected_cell(const coordinates &coords);


    /**
     * Displays the editor canvas on the screen.
     */
    void update();


    /**
     * Places the cursor to specified coordinates.
     *
     * @param _t::editor::coordinates & coords
     * @param bool                      selection
     */
    void cursor_move(const coordinates &coords, bool selection = false);

    /**
     * Scrolls to the cursor, if not already in the viewport.
     */
    void scroll_to_cursor();

    /**
     * Scrolls to the end of the viewport if the text area ends above.
     */
    void safe_scroll_end();


    /**
     * Displays the cursor on the editor canvas.
     */
    void cursor_show();

    /**
     * Hides the cursor from the editor canvas.
     */
    void cursor_hide();

    /**
     * Activates the cursor blinking.
     */
    void cursor_activate();

    /**
     * Deactivates the cursor blinking.
     */
    void cursor_deactivate();


    /**
     * Increases the editors font size by /n/.
     *
     * @param qint32 n
     */
    void increase_font_size(qint32 n = 1);

    /**
     * Decreases the editors font size by /n/.
     *
     * @param qint32 n
     */
    void decrease_font_size(qint32 n = 1);


    /**
     * Applies a function to each cell of the coordinates range.
     * Range coordinates must be in ascending order.
     *
     * @param const _t::editor::coordinates                              & start
     * @param const _t::editor::coordinates                              & end
     * @param       std::function<void(const _t::editor::coordinates &)>   func
     */
    void each_cell(
        const coordinates &start,
        const coordinates &end,
        std::function<void(const coordinates &)> func);

private slots:
    /**
     * Handles any user input from mouse buttons.
     *
     * @param QMouseEvent * event
     */
    void mouse_pressed(QMouseEvent *event);

    /**
     * Handles mouse movement.
     *
     * @param QMouseEvent * event
     */
    void mouse_moved(QMouseEvent *event);


    /**
     * Handles showing/hiding cursor when blinking.
     */
    void cursor_timer_ticked();


    /**
     * Handles shifting the text after vertical scroll.
     */
    void vscrolled();

public:
    /**
     * The main constructor.
     */
    editor();

    /**
     * The main destructor.
     */
    ~editor();


    /**
     * Writes the text into editor's memory and displays it on the screen.
     *
     * @param const QString & text
     */
    void write(const QString &text);

    /**
     * Moves the cursor to the beginning of the text.
     */
    void move_cursor_to_beginning();

    /**
     * Returns the joined text list as a string.
     *
     * @return QString
     */
    QString get_text() const;


    /**
     * @param const QVector<QPair<QColor, QString>> * rules
     */
    void set_sh_rules(const QVector<QPair<QColor, QString>> *rules);


    /**
     * Displays the find dialog.
     */
    void show_find_dialog();

    /**
     * Displays the replace dialog.
     */
    void show_replace_dialog();


    /**
     * Finds the next occurence of the string in the editor's text.
     *
     * @param const QString & pattern
     * @param       bool      regex
     */
    void find_next(const QString &pattern, bool regex);

    /**
     * Replaces all occurences in the string.
     *
     * @param const QString & find
     * @param const QString & replace
     * @param       bool      case_sensitive
     * @param       bool      regex
     */
    void replace(
        const QString &find,
        const QString &replace,
        bool case_sensitive,
        bool regex);
};

#endif // _T_EDITOR_EDITOR_H
