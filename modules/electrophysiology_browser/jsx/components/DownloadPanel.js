/**
 * This file contains React component for Electrophysiology module.
 */
import React, {Component} from 'react';
import Panel from 'Panel';

/**
 * EEG Download Panel
 *
 * Display EEG files fto download
 */
class DownloadPanel extends Component {
  /**
   * @constructor
   * @param {object} props - React Component properties
   */
  constructor(props) {
    super(props);
    this.state = {
      data: this.props.data,
      labels: {
        physiological_file: 'EEG File',
        physiological_electrode_file: 'Electrode Info',
        physiological_channel_file: 'Channels Info',
        physiological_task_event_file: 'Events',
        all_files: 'All Files',
        physiological_fdt_file: '',
      },
    };
  }

  /**
   * Renders the React component.
   *
   * @return {JSX} - React markup for the component
   */
  render() {
    return (
      <Panel
        id='file-download'
        title={'File Download'}
      >
        <div style={{
          minHeight: '330px',
          display: 'flex',
          flexDirection: 'column',
          justifyContent: 'center',
        }}>
          {this.state.data.downloads
            .filter((download) =>
              download.type != 'physiological_fdt_file'
            )
            .map((download, i) => {
              const disabled = (download.file === '') ? true : false;
              return (
                <div
                  key={i}
                  className={'form-group row'}
                >
                  <div
                    className={'col-xs-offset-3 col-xs-4'}
                    style={{
                      color: '#074785',
                      fontWeight: 'bold',
                      lineHeight: '30px',
                      verticalAlign: 'middle',
                    }}
                  >{this.state.labels[download.type]}</div>
                  {disabled
                    ? <a
                        className='btn disabled'
                        style={{
                          width: '120px',
                          color: '#b3b3b3',
                          cursor: 'not-allowed',
                          borderRadius: '10px',
                          border: '1px solid #b3b3b3',
                        }}
                      >Not Available</a>
                    : <a
                        className='btn'
                        href={'/mri/jiv/get_file.php?file=' + download.file}
                        target='_blank'
                        download={this.state.data.downloads[0].file}
                        style={{
                          width: '120px',
                          outline: 'none',
                          color: '#1c4781',
                          borderRadius: '10px',
                          textDecoration: 'none',
                          border: '1px solid #1c4781',
                        }}
                      >Download</a>
                  }
                </div>
              );
            })
          }
        </div>
      </Panel>
    );
  }
}

export {DownloadPanel};
