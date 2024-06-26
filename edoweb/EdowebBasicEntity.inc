<?php
/**
 * Copyright 2013 hbz NRW (http://www.hbz-nrw.de/)
 *
 * This file is part of regal-drupal.
 *
 * regal-drupal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * regal-drupal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with regal-drupal.  If not, see <http://www.gnu.org/licenses/>.
 */

class EdowebBasicEntity extends Entity {

  private $__source = null;

  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);
    // Workaround for entity_js module which expects
    // bundle name in type property
    $this->type = $this->bundle();
  }

  protected function defaultUri() {
    if (isset($this->remote_id)) {
      return array(
        'path' => "resource/{$this->remote_id}",
      );
    }
  }

  public function url() {
    return url("resource/{$this->remote_id}", array('absolute' => TRUE));
  }

  public function identifier() {
    if (isset($this->remote_id) && _is_edoweb_entity($this)) {
      $remote_id = $this->remote_id;
    } else if (isset($this->remote_id)) {
      $remote_id = _edoweb_expand_curie($this->remote_id);
    } else {
      $remote_id = null;
    }
    return $remote_id;
  }

  public function internalIdentifier() {
    return isset($this->remote_id)
      ? $this->remote_id
      : null;
  }

  public function label($view_mode = 'full') {
    $label_fields = array();

    switch ($this->bundle_type) {
      case 'monograph':
      case 'journal':
      case 'issue':
      case 'article':
      case 'collection':
      case 'volume':
      case 'file':
      case 'webpage':
      case 'version':
      case 'part':
        $label_fields[] = 'field_edoweb_title';
        break;
      case 'person':
      case 'corporate_body':
      case 'subject':
      case 'event':
      case 'family':
      case 'work':
      case 'place':
      case 'authority_resource':
        $label_fields[] = 'field_gnd_name';
        break;
      default:
        $label_fields[] = 'field_edoweb_title';
        $label_fields[] = 'field_edoweb_label';
        break;
    }

    if(!isset($this->remote_id))
        return "";
    return (empty($this->stringifyFields($label_fields) && isset($this->remote_id))
      ? _edoweb_expand_curie($this->remote_id)
      : implode(' / ', $this->stringifyFields($label_fields)));
  }

  public function subLabel($view_mode = 'compact') {
    $label_fields = array();
    if( variable_get('sub_title_fields') == null ) {
	drupal_set_message("WARN: sub_title_fields sind leer !");
        return null;
    }
    foreach (variable_get('sub_title_fields') as $field => $enabled) {
      if ($enabled) {
        $label_fields[] = $field;
      }
    }
    return implode(' ' . variable_get('sub_title_field_separator') . ' ', $this->stringifyFields($label_fields));
  }
  public function getField($fieldName){
    return implode($this->stringifyFields($fieldName));
  }
  private function stringifyFields($field_names) {
    $wrapper = entity_metadata_wrapper('edoweb_basic', $this);
    $title_values = array();
    foreach ($field_names as $label_field) {
      try {
        $title_value = $wrapper->$label_field->value();
        $values = array();
        if (is_array($title_value)) {
          foreach($title_value as $value) {
            if (is_array($value)) {
              $render = field_view_value(
                EDOWEB_ENTITY_TYPE, $this, $label_field, $value
              );
            } else {
              $render = field_view_value(
                EDOWEB_ENTITY_TYPE, $this, $label_field, array('value' => $value)
              );
            }
            $values[] = drupal_render($render);
          }
        } else {
          $render = field_view_value(
            EDOWEB_ENTITY_TYPE, $this, $label_field, array('value' => $title_value)
          );
          $values[] =  html_entity_decode(drupal_render($render),ENT_QUOTES);
        }
  
        if ($title_value) $title_values[] = implode(' ' . variable_get('sub_title_field_value_separator') . ' ', $values);
      } catch (EntityMetadataWrapperException $e) {
         //drupal_set_message($e);
      }
    }
    return $title_values;
  }

  public function pageTitle(){
	$api = new EdowebAPIClient();
  	$htmlString=$api->getTitle($this->remote_id);
	return $htmlString;
  }
}
