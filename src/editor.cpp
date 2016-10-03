#include "editor.h"

#include <QApplication>
#include <QClipboard>


void _t::editor::init(QWidget *parent_widget)
{
    this->setParent(parent_widget);

    // todo: start maximized
    this->resize(1280, 720);

    this->background = QColor(60, 60, 65);

    this->font = QFont("Consolas", 12);
    this->font_color = QColor(250, 250, 250);

    this->cursor_timer.setParent(this);
    connect(&this->cursor_timer, SIGNAL(timeout()), this, SLOT(cursor_timer_tick()));
    this->cursor_timer.setInterval(500);

    this->area.setParent(this);
    this->area.resize(this->width(), this->height());
    connect(&this->area, SIGNAL(clicked(QPoint)), this, SLOT(area_clicked(QPoint)));

    // todo: same size as 'this'
    this->canvas = QPixmap(1280, 720);
    this->canvas.fill(this->background);

    this->setCursor(Qt::IBeamCursor);

    this->update();

    this->text << "";

    this->setFocus();
}


void _t::editor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
    {
        this->cursor_deactivate();

        // there is a line above
        if (this->cursor.row > 0)
        {
            --this->cursor.row;

            qint32 line_length = this->active_line().length();

            // cursor is after the end of the line
            if (this->cursor.col > line_length)
            {
                this->cursor.col = line_length;
            }
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Right)
    {
        this->cursor_deactivate();

        // cursor is at the end of the line
        if (this->cursor.col == this->active_line().length())
        {
            // cursor is not at the last line
            if (this->text.count() > this->cursor.row + 1)
            {
                ++this->cursor.row;
                this->cursor.col = 0;
            }
        }
        else
        {
            ++this->cursor.col;
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Down)
    {
        this->cursor_deactivate();

        // there is a line below
        if (this->text.count() > this->cursor.row + 1)
        {
            ++this->cursor.row;

            qint32 line_length = this->active_line().length();

            // cursor is after the end of the line
            if (this->cursor.col > line_length)
            {
                this->cursor.col = line_length;
            }
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Left)
    {
        this->cursor_deactivate();

        // cursor is at the beginning of the line
        if (this->cursor.col == 0)
        {
            // cursor is not at the first line
            if (this->cursor.row != 0)
            {
                --this->cursor.row;
                this->cursor.col = this->active_line().length();
            }
        }
        else
        {
            --this->cursor.col;
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Home)
    {
        this->cursor_deactivate();

        if (this->active_line().length() > 0)
        {
            if (this->cursor.col == 0)
            {
                while (this->active_line().at(this->cursor.col) == ' ')
                {
                    ++this->cursor.col;
                }
            }
            else
            {
                while (this->active_line().at(--this->cursor.col) == ' ' && this->cursor.col);

                if (this->cursor.col != 0)
                {
                    this->cursor.col = 0;

                    while (this->active_line().at(this->cursor.col) == ' ')
                    {
                        ++this->cursor.col;
                    }
                }
            }
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_End)
    {
        this->cursor_deactivate();

        this->cursor.col = this->active_line().length();

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        this->write(this->newline_character);
    }

    else if (event->key() == Qt::Key_Backspace)
    {
        this->cursor_deactivate();

        // cursor is not at the beginning of the line
        if (this->cursor.col > 0)
        {
            --this->cursor.col;

            QPainter painter(&this->canvas);
            this->setup_painter_clear(painter);

            this->clear_cell(painter);

            // cursor is not at the end of the line
            if (this->cursor.col + 1 < this->active_line().length())
            {
                // move the rest of the active line one cell left
                painter.drawPixmap(
                    this->cursor.col * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->canvas,
                    (this->cursor.col + 1) * this->cell_width,
                    this->cursor.row * this->cell_height,
                    (this->active_line().length() - this->cursor.col) * this->cell_width,
                    this->cell_height);

                this->clear_cell(painter, this->cursor.row, this->active_line().length() - 1);
            }

            this->update();

            this->active_line().remove(this->cursor.col, 1);
        }

        // cursor is at the beginning of the line
        else
        {
            if (this->cursor.row > 0)
            {
                --this->cursor.row;
                this->cursor.col = this->active_line().length();

                QPainter painter(&this->canvas);
                this->setup_painter_clear(painter);

                // original line isn't empty
                if (this->text[this->cursor.row + 1].length() > 0)
                {
                    // move the remaining text to the end of the line
                    painter.drawPixmap(
                        this->cursor.col * this->cell_width,
                        this->cursor.row * this->cell_height,
                        this->canvas,
                        0,
                        (this->cursor.row + 1) * this->cell_height,
                        this->text[this->cursor.row + 1].length() * this->cell_width,
                        this->cell_height);

                    painter.drawRect(
                        0,
                        (this->cursor.row + 1) * this->cell_height,
                        this->text[this->cursor.row + 1].length() * this->cell_width,
                        this->cell_height);

                    this->active_line().append(this->text[this->cursor.row + 1]);
                }

                // move all following lines one row up
                for (qint32 i = this->cursor.row + 2; i < this->text.count(); ++i)
                {
                    qint32 line_length = this->text[i].length();

                    painter.drawPixmap(
                        0,
                        (i - 1) * this->cell_height,
                        this->canvas,
                        0,
                        i * this->cell_height,
                        line_length * this->cell_width,
                        this->cell_height);

                    painter.drawRect(0, i * this->cell_height, line_length * this->cell_width, this->cell_height);
                }

                this->update();

                this->text.removeAt(this->cursor.row + 1);
            }
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Delete)
    {
        this->cursor_deactivate();

        // cursor is not at the end of the line
        if (this->cursor.col < this->active_line().length())
        {
            QPainter painter(&this->canvas);
            this->setup_painter_clear(painter);

            this->clear_cell(painter);

            // cursor is not before the last letter in the line
            if (this->cursor.col + 1 < this->active_line().length())
            {
                // move the rest of the active line one cell left
                painter.drawPixmap(
                    this->cursor.col * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->canvas,
                    (this->cursor.col + 1) * this->cell_width,
                    this->cursor.row * this->cell_height,
                    (this->active_line().length() - this->cursor.col) * this->cell_width,
                    this->cell_height);

                this->clear_cell(painter, this->cursor.row, this->active_line().length() - 1);
            }

            this->update();

            this->active_line().remove(this->cursor.col, 1);
        }

        // cursor is at the end of the line
        else
        {
            // cursor is not at the last line
            if (this->cursor.row + 1 < this->text.length())
            {
                QPainter painter(&this->canvas);
                this->setup_painter_clear(painter);

                // next line isn't empty
                if (this->text[this->cursor.row + 1].length() > 0)
                {
                    // move the next line to the end of this line
                    painter.drawPixmap(
                        this->cursor.col * this->cell_width,
                        this->cursor.row * this->cell_height,
                        this->canvas,
                        0,
                        (this->cursor.row + 1) * this->cell_height,
                        this->text[this->cursor.row + 1].length() * this->cell_width,
                        this->cell_height);

                    painter.drawRect(
                        0,
                        (this->cursor.row + 1) * this->cell_height,
                        this->text[this->cursor.row + 1].length() * this->cell_width,
                        this->cell_height);

                    this->active_line().append(this->text[this->cursor.row + 1]);
                }

                // move all following lines one row up
                for (qint32 i = this->cursor.row + 2; i < this->text.count(); ++i)
                {
                    qint32 line_length = this->text[i].length();

                    painter.drawPixmap(
                        0,
                        (i - 1) * this->cell_height,
                        this->canvas,
                        0,
                        i * this->cell_height,
                        line_length * this->cell_width,
                        this->cell_height);

                    painter.drawRect(0, i * this->cell_height, line_length * this->cell_width, this->cell_height);
                }

                this->update();

                this->text.removeAt(this->cursor.row + 1);
            }
        }

        this->cursor_activate();
    }

    else if (event->key() == Qt::Key_Tab)
    {
        this->write("    ");
    }

    else if (event->key() == Qt::Key_V)
    {
        if (event->modifiers() == Qt::ControlModifier)
        {
            this->write(QApplication::clipboard()->text());
        }
        else
        {
            this->write(event->text());
        }
    }

    else
    {
        this->write(event->text());
    }
}

void _t::editor::area_clicked(QPoint coords)
{
    this->cursor_deactivate();

    // todo: set cursor only if clicked by LMB

    qint32 row = coords.y() / this->cell_height;
    qint32 col = coords.x() / this->cell_width;
    if (coords.x() % this->cell_width > this->cell_width / 2)
    {
        ++col;
    }

    // clicked below the last line
    if (row >= this->text.count())
    {
        this->cursor.row = this->text.count() - 1;
    }
    else
    {
        this->cursor.row = row;
    }

    // clicked after the end of the line
    if (col > this->active_line().length())
    {
        this->cursor.col = this->active_line().length();
    }
    else
    {
        this->cursor.col = col;
    }

    this->cursor_activate();
}

void _t::editor::focusInEvent(QFocusEvent *)
{
    this->cursor_activate();
}

void _t::editor::focusOutEvent(QFocusEvent *)
{
    this->cursor_deactivate();
}


void _t::editor::write(const QString &text)
{
    this->cursor_deactivate();

    QPainter painter(&this->canvas);

    for (QChar character : text)
    {
        if (character == this->newline_character)
        {
            qint32 active_line_rest_length = this->active_line().length() - this->cursor.col;

            this->setup_painter_clear(painter);

            // cursor is not at the last line
            if (this->cursor.row + 1 < this->text.count())
            {
                // move all following lines one row down
                for (qint32 i = this->text.count() - 1; i > this->cursor.row; --i)
                {
                    painter.drawPixmap(
                        0,
                        (i + 1) * this->cell_height,
                        this->canvas,
                        0,
                        i * this->cell_height,
                        this->cell_width * this->text[i].length(),
                        this->cell_height);

                    painter.drawRect(
                        0,
                        i * this->cell_height,
                        this->cell_width * this->text[i].length(),
                        this->cell_height);
                }
            }

            // cursor is at the end of the line
            if (this->cursor.col == this->active_line().length())
            {
                this->text.insert(this->cursor.row + 1, "");
            }
            else
            {
                // create a new line with the rest of the original line
                this->text.insert(this->cursor.row + 1, this->active_line().right(active_line_rest_length));
                this->active_line().chop(active_line_rest_length);

                // move the rest of the active line to the beginning of the next line
                painter.drawPixmap(
                    0,
                    (this->cursor.row + 1) * this->cell_height,
                    this->canvas,
                    this->cursor.col * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->cell_width * active_line_rest_length,
                    this->cell_height);

                painter.drawRect(
                    this->cursor.col * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->cell_width * active_line_rest_length,
                    this->cell_height);
            }

            ++this->cursor.row;
            this->cursor.col = 0;
        }

        // character is not a newline
        else
        {
            // cursor is not at the end of line
            if (this->cursor.col < this->active_line().length())
            {
                this->setup_painter_clear(painter);

                // move the rest of the active line one cell right
                painter.drawPixmap(
                    (this->cursor.col + 1) * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->canvas,
                    this->cursor.col * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->cell_width * (this->active_line().length() - this->cursor.col),
                    this->cell_height);

                painter.drawRect(
                    this->cursor.col * this->cell_width,
                    this->cursor.row * this->cell_height,
                    this->cell_width,
                    this->cell_height);
            }

            this->setup_painter_write(painter);

            painter.drawText(
                this->cursor.col * this->cell_width,
                this->cursor.row * this->cell_height,
                this->cell_width,
                this->cell_height,
                0,
                character);

            this->active_line().insert(this->cursor.col, character);

            ++this->cursor.col;
        }
    }

    painter.end();

    this->update();

    this->cursor_activate();
}


QString &_t::editor::active_line()
{
    return this->text[this->cursor.row];
}

void _t::editor::setup_painter_clear(QPainter &painter)
{
    painter.setPen(this->background);
    painter.setBrush(QBrush(this->background));
}

void _t::editor::setup_painter_write(QPainter &painter)
{
    painter.setFont(this->font);
    painter.setPen(this->font_color);
}

void _t::editor::clear_cell(QPainter &painter, qint32 row, qint32 col)
{
    painter.drawRect(col * this->cell_width, row * this->cell_height, this->cell_width, this->cell_height);
}

void _t::editor::clear_cell(QPainter &painter)
{
    this->clear_cell(painter, this->cursor.row, this->cursor.col);
}

void _t::editor::update()
{
    this->area.setPixmap(this->canvas);
}


void _t::editor::cursor_show()
{
    if (!this->cursor_visible)
    {
        this->cursor.background = this->canvas.copy(
            this->cursor.col * this->cell_width,
            this->cursor.row * this->cell_height,
            this->cell_width,
            this->cell_height);

        QPainter painter(&this->canvas);

        painter.fillRect(
            this->cursor.col * this->cell_width,
            this->cursor.row * this->cell_height,
            2,
            this->cell_height,
            QColor(50, 170, 110));

        this->update();

        this->cursor_visible = true;
    }
}

void _t::editor::cursor_hide()
{
    if (this->cursor_visible)
    {
        QPainter painter(&this->canvas);

        painter.drawPixmap(
            this->cursor.col * this->cell_width,
            this->cursor.row * this->cell_height,
            this->cell_width,
            this->cell_height,
            this->cursor.background);

        this->update();

        this->cursor_visible = false;
    }
}

void _t::editor::cursor_activate()
{
    this->cursor_show();

    this->cursor_timer.start();
}

void _t::editor::cursor_deactivate()
{
    this->cursor_timer.stop();

    this->cursor_hide();
}


void _t::editor::cursor_timer_tick()
{
    if (this->cursor_visible)
    {
        this->cursor_hide();
    }
    else
    {
        this->cursor_show();
    }
}